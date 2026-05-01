
#ifndef __MYOS__HARDWARECOMMUNICATION__PCI_H
#define __MYOS__HARDWARECOMMUNICATION__PCI_H
    #include <hardwarecommunication/port.h>
    #include <drivers/driver.h>
    #include <common/types.h>
    #include <hardwarecommunication/interrupts.h>

    typedef struct PeripheralComponentInterconnectDeviceDescriptor
    {
        uint32_t portBase;
        uint32_t interrupt;

        uint16_t bus;
        uint16_t device;
        uint16_t function;

        uint16_t vendor_id;
        uint16_t device_id;

        uint8_t class_id;
        uint8_t subclass_id;
        uint8_t interface_id;

        uint8_t revision;
    } pci_device_descriptor_t;

    typedef struct PeripheralComponentInterconnectController
    {
        port32bit_t dataPort;
        port32bit_t commandPort;
    } pci_controller_t;

    void pci_init(pci_controller_t* pci);

    uint32_t pci_read(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
    void pci_write(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value);

    int pci_device_has_function(pci_controller_t* pci, uint16_t bus, uint16_t device);
    void pci_select_drivers(pci_controller_t* pci, driver_manager_t* driverManger);

    pci_device_descriptor_t pci_read_descriptor(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function);



#endif


