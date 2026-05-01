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

pci_device_descriptor_t pci_device_get_descriptor(pci_controller_t* pci, uint8_t bus, uint8_t device, uint8_t function)
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

void pci_select_drivers(pci_controller_t* pci, driver_manager_t* driverManager)
{
    for (int bus = 0; bus < 8; bus++)
        for (int device = 0; device < 32; device++)
        {
            int numFunctions = pci_device_has_function(pci, bus, device) ? 8 : 1;

            for (int function = 0; function < numFunctions; function++)
            {
                pci_device_descriptor_t dev = pci_device_get_descriptor(pci, bus, device, function);

                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    break;

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