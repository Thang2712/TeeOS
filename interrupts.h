
#ifndef __INTERRUPTMANAGER_H
#define __INTERRUPTMANAGER_H

    #include "gdt.h"
    #include "types.h"
    #include "port.h"

    /*
     * Structure of an IDT Gate Descriptor (8 bytes).
     * Defines how the CPU should jump to a handler function when an interrupt occurs.
     */
    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;             // Lower 16 bits of the handler function address
        uint16_t gdt_codeSegmentSelector;           // The Code Segment selector from GDT (usually 0x10)
        uint8_t reserved;                           // Always set to 0
        uint8_t access;                             // Present bit, Privilege level, and Gate Type (e.g., 0x8E)
        uint16_t handlerAddressHighBits;            // Higher 16 bits of the handler function address
    } __attribute__((packed));

    /*
     * Structure used by the 'lidt' instruction to load the IDT into the CPU.
     */
    struct InterruptDescriptorTablePointer
    {
        uint16_t size;      // The total size of the IDT in byte minus 1
        uint32_t base;      // The linear starting address of the GateDescriptor array
    } __attribute__((packed));

    /*
     * Structure representing an interrupt handler.
     */
    struct InterruptHandler
    {
        uint32_t (*Handle)(struct InterruptHandler* self, uint32_t esp);
        void* instance;
    };

    /*
     * Manager hardware interrupts via the Programmable Interrupt Controllers (PIC).
     */
    struct InterruptManager
    {
        uint16_t hardwareInterruptOffset;           // Base offset for IRQs (e.g., 0x20 to avoid Exceptions)
        struct Port8BitSlow picMasterCommand;       // Command port for Master PIC (0x20)
        struct Port8BitSlow picMasterData;          // Data port for Master PIC (0x21)
        struct Port8BitSlow picSlaveCommand;        // Command port for Slave PIC (0xA0)
        struct Port8BitSlow picSlaveData;           // Data port for Slave PIC (0xA1)
        struct InterruptHandler* handlers[256];     // Array of handlers for each interrupt (0-255)
    };

    /*
     * CPU Exception Handlers (interrupts 0x00 - 0x13)
     * Defined in assembly (interruptstubs.s)
     */
    extern void HandleException0x00();
    extern void HandleException0x01();
    extern void HandleException0x02();
    extern void HandleException0x03();
    extern void HandleException0x04();
    extern void HandleException0x05();
    extern void HandleException0x06();
    extern void HandleException0x07();
    extern void HandleException0x08();
    extern void HandleException0x09();
    extern void HandleException0x0A();
    extern void HandleException0x0B();
    extern void HandleException0x0C();
    extern void HandleException0x0D();
    extern void HandleException0x0E();
    extern void HandleException0x0F();
    extern void HandleException0x10();
    extern void HandleException0x11();
    extern void HandleException0x12();
    extern void HandleException0x13();

    /*
     * Hardware Interrupt Request (IRQ) Handlers (usually start at 0x20)
     * Defined in assembly (interruptstubs.s)
     */
    extern void HandleInterruptRequest0x00();   // Timer
    extern void HandleInterruptRequest0x01();   // Keyboard
    extern void HandleInterruptRequest0x02();
    extern void HandleInterruptRequest0x03();
    extern void HandleInterruptRequest0x04();
    extern void HandleInterruptRequest0x05();
    extern void HandleInterruptRequest0x06();
    extern void HandleInterruptRequest0x07();
    extern void HandleInterruptRequest0x08();   // Real-time clock
    extern void HandleInterruptRequest0x09();
    extern void HandleInterruptRequest0x0A();
    extern void HandleInterruptRequest0x0B();
    extern void HandleInterruptRequest0x0C();   // Mouse
    extern void HandleInterruptRequest0x0D();
    extern void HandleInterruptRequest0x0E();   // Primary ATA
    extern void HandleInterruptRequest0x0F();   // Secondary ATA
    extern void HandleInterruptRequest0x31();   // System Call / Custom

    /*
     * Default handler for unhandled or ignored interrupts.
     */
    extern void InterruptIgnore();


    /*
     * Initializes the IDT and configures the PICs to remap IRQs.
     */
    void init_interrupt_manager(struct InterruptManager* am, uint16_t hardwareInterruptOffset, struct GlobalDescriptorTable* gdt);

    /*
     * Enables hardware interrupts by executing the 'sti' instruction.
     */
    void activate_interrupts();

    /*
     * Disables hardware interrupts by executing the 'cli' instruction.
     */
    void deactivate_interrupts();

    /*
     * The high-level C function called assembly stubs to process and interrupt.
     * @param interrupt The interrupt number triggered
     * @param esp The current stack pointer containing register states.
     */
    uint32_t HandleInterrupt(uint32_t interrupt, uint32_t esp);
#endif