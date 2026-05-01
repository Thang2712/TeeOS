#ifndef __MYOS__HARDWARECOMMUNICATION__PCI_H
#define __MYOS__HARDWARECOMMUNICATION__PCI_H

    #include <hardwarecommunication/port.h>
    #include <drivers/driver.h>
    #include <common/types.h>
    #include <hardwarecommunication/interrupts.h>
    #include <stdbool.h>

    /*
     * @brief PCI Base Address Register(BAR) types.
     * Determines if the devices is mapped into Memory spaces and I/O space.
     */
    typedef enum
    {
        MemoryMapping = 0,
        InputOutput = 1
    } pci_bar_type_t;

    /*
     * @brief Structure representing a PCI Base Address Register (BAR).
     * Contains physical address, memory size, and mapping type.
     */
    typedef struct
    {
        bool prefetchable;          // Indicates if the data can be cached/pre-read by CPU
        uint8_t* address;           // The base physical address
        uint32_t size;              // Size of the address space in bytes
        pci_bar_type_t type;        // BAR type (Memory or I/O)
    } pci_bar_t;

    /*
     * @brief PCI Device Descriptor.
     * Holds identification and configuration data for a specific PCI device.
     */
    typedef struct PeripheralComponentInterconnectDeviceDescriptor 
    {
        uint32_t portBase;          // I/O port base address (usually from BAR)
        uint32_t interrupt;         // Interrupt Request (IRQ) number used by the device
        uint16_t bus;               // PCI Bus number (0-255)
        uint16_t device;            // Device number on the bus (0-31)
        uint16_t function;          // Function number on the device (0-7)
        uint16_t vendor_id;         // Manufacturer ID (e.g., 0x1022 for AMD)
        uint16_t device_id;         // Device-specific identifier
        uint8_t class_id;           // Major device category (e.g., 0x03 for Display)
        uint8_t subclass_id;        // Minor device category (e.g., 0x00 for VGA)
        uint8_t interface_id;       // Specific register-level programming interface
        uint8_t revision;           // Hardware revision number
    } pci_device_descriptor_t;

    /*
     * @brief PCI Controller structure.
     * Manages the I/O ports used to access the PCI Configuration Space.
     */
    typedef struct PeripheralComponentInterconnectController 
    {
        port32bit_t dataPort;       // Port 0xCFC: Data transfer port
        port32bit_t commandPort;    // Port 0xCF8: Address/Command port
    } pci_controller_t;

    //Function Prototypes

    // @brief Initializes the PCI controller with default I/O ports.
    void pci_init(pci_controller_t* pci);
    // @brief Reads a 32-bit register from a specific PCI device's configuration space.
    uint32_t pci_read(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset);
    // @brief Writes a 32-bit value to a specific PCI device's configuration register
    void pci_write(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value);
    // @brief Checks if a PCI device supports multiple functions
    int pci_device_has_function(pci_controller_t* pci, uint16_t bus, uint16_t device);
    // @brief Scans the PCI bus and selects appropriate drivers for detected devices.
    void pci_select_drivers(pci_controller_t* pci, driver_manager_t* driverManager, interrupt_manager_t* interrupts);
    // @brief Retrieves the full descriptor for a give PCI bus, device, and function.
    pci_device_descriptor_t pci_get_device_descriptor(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function);
    // @brief Reads the Base Address Register (BAR) for a specific device
    pci_bar_t pci_get_base_address_register(pci_controller_t* pci, uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);
    // @brief Instantiates a driver base on the device descriptor and interrupt manager.
    driver_t* pci_get_driver(pci_device_descriptor_t dev, interrupt_manager_t* interrupts);

#endif