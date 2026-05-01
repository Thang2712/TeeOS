#include <hardwarecommunication/pci.h>


void kprintf(char* str);
void printfHex(uint8_t);

void pci_init(pci_controller_t* pci)
{
    init_port32bit(&pci->commandPort, 0xCF8);
    init_port32bit(&pci->dataPort, 0xCFC);
}

uint32_t pci_read(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    uint32_t id = 0x1 << 31
                | ((bus & 0xFF) << 16)
                | ((device & 0x1F) << 11)
                | ((function & 0x07) << 8)
                | (registeroffset & 0xFC);

    pci->commandPort.Write(&pci->commandPort, id);
    uint32_t result = pci->dataPort.Read(&pci->dataPort);

    return result >> (8 * (registeroffset % 4));
}

void pci_write(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value) {
    uint32_t id = 0x1 << 31
                | ((bus & 0xFF) << 16)
                | ((device & 0x1F) << 11)
                | ((function & 0x07) << 8)
                | (registeroffset & 0xFC);

    pci->commandPort.Write(&pci->commandPort, id);
    pci->dataPort.Write(&pci->dataPort, value);
}

int pci_device_has_function(pci_controller_t* pci, uint16_t bus, uint16_t device)
{
    return pci_read(pci, bus, device, 0, 0x0E) & (1 << 7);
}

pci_bar_t pci_get_base_address_register(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
    pci_bar_t result;
    result.address = 0;

    uint32_t headertype = pci_read(pci, bus, device, function, 0x0E) & 0x7F;
    int max_bars = 6 - (4 * headertype);
    if (bar >= max_bars)
        return result;

    uint32_t bar_value = pci_read(pci, bus, device, function, 0x10 + (4 * bar));
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;

    if (result.type == MemoryMapping)
    {
        result.address = (uint8_t*)(bar_value & ~0xF);
        result.prefetchable = (bar_value >> 3) & ~0x1; 
    }
    else
    {
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false; 
    }

    return result;
} 


pci_device_descriptor_t pci_get_device_descriptor(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function)
{
    pci_device_descriptor_t result;


    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = pci_read(pci, bus, device, function, 0x00);
    result.device_id = pci_read(pci, bus, device, function, 0x02);

    result.class_id = pci_read(pci, bus, device, function, 0x0b);
    result.subclass_id = pci_read(pci, bus, device, function, 0x0a);
    result.interface_id = pci_read(pci, bus, device, function, 0x09);

    result.revision = pci_read(pci, bus, device, function, 0x08);
    result.interrupt = pci_read(pci, bus, device, function, 0x3c);

    return result;
}

driver_t* pci_get_driver(pci_device_descriptor_t dev, interrupt_manager_t* interrupts)
{
    driver_t* driver = 0; 

    switch (dev.vendor_id)
    {
        case 0x1022: 
            if (dev.device_id == 0x2000)
                kprintf("AMD am79c973"); 
            break; 
        case 0x8086:
            break; 
    }

    switch (dev.class_id)
    {
        case 0x03: 
            if (dev.subclass_id == 0x00)
                kprintf("VGA");
            break;

    }
    return driver;
}

void pci_select_drivers(pci_controller_t* pci, driver_manager_t* driverManager, interrupt_manager_t* interrupt)
{
    for (int bus = 0; bus < 8; bus++)
        for (int device = 0; device < 32; device++)
        {
            int numFunctions = pci_device_has_function(pci, bus, device) ? 8 : 1;

            for (int function = 0; function < numFunctions; function++)
            {
                pci_device_descriptor_t dev = pci_get_device_descriptor(pci, bus, device, function);

                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    break;
                
                for (int barNum = 0; barNum < 6; barNum++)
                {
                    pci_bar_t bar = pci_get_base_address_register(pci, bus, device, function, barNum);
                    if (bar.address && (bar.type == InputOutput))
                        dev.portBase = (uint32_t)bar.address;  
                }

                driver_t* driver = pci_get_driver(dev, interrupt); 
                if (driver != 0)
                    driver_manager_add_driver(driverManager, driver);

                kprintf("PCI BUS");
                printfHex(bus & 0xFF);

                kprintf(", DEVICE ");
                printfHex(device & 0xFF);

                kprintf(", FUNCTION ");
                printfHex(function & 0xFF);

                kprintf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);

                kprintf(", DEVICE ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                kprintf("\n");
            }
        }
}