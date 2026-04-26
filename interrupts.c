
#include "interrupts.h"

// IDT Pointer must be static or global to ensure it persists in memory
static struct InterruptDescriptorTablePointer idt_pointer;

void kprintf(char *str);

// The actual IDT table containing 256 interrupt gates
struct GateDescriptor interruptDescriptorTable[256];

/*
 * Configures a single entry in the Interrupt Descriptor Table.
 * @param interrupt The interrupt number (0-255)
 * @param CodeSegment The GDT selector for the code segment (usually 0x08)
 * @param handler Pointer to the assembly stub handling the interrupt
 * @param DescriptorPrivilegeLevel CPU privilege level (0 for kernel, 3 for user)
 * @param DescriptorType Type of gate (0xE for 32-bits Interrupt Gate)
 */
void SetInterruptDescriptorTableEntry(uint8_t interrupt, uint16_t CodeSegment, void (*handler) (), uint8_t DescriptorPrivilegeLevel, uint8_t DescriptorType)
{
    uint32_t handlerAddress = (uint32_t)handler;

    // Split the 32-bits handler address into low and high 16-bits parts
    interruptDescriptorTable[interrupt].handlerAddressLowBits = handlerAddress & 0xFFFF;
    interruptDescriptorTable[interrupt].handlerAddressHighBits = (handlerAddress >> 16) & 0xFFFF;
    interruptDescriptorTable[interrupt].gdt_codeSegmentSelector = CodeSegment;
    interruptDescriptorTable[interrupt].reserved = 0;

    // Set access flags: Present bit | Privilege Level | Gate Type
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interrupt].access = IDT_DESC_PRESENT | ((DescriptorPrivilegeLevel & 3) << 5) | DescriptorType;

}

/*
 * Initializes the Interrup Manager, remaps the PIC, and loads the IDT.
 */
void init_interrupt_manager(struct InterruptManager* am, uint16_t hardwareInterruptOffset, struct GlobalDescriptorTable* gdt)
{
    am->hardwareInterruptOffset = hardwareInterruptOffset;

    uint16_t CodeSegment = 0x10;        // Usually 0x08 for Kernel Code Segment

    const uint8_t IDT_INTERRUPT_GATE = 0xE;
    const uint8_t KERNEL_PRIVILEGE = 0;

    // Initialize all 256 entries with a default 'Ignore' handler
    for (int i = 0; i < 256; i++)
        SetInterruptDescriptorTableEntry(i, CodeSegment, &InterruptIgnore, KERNEL_PRIVILEGE, IDT_INTERRUPT_GATE);

    // Setup Exception handlers (0x00 - 0x13)
    SetInterruptDescriptorTableEntry(0x00, CodeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x01, CodeSegment, &HandleException0x01, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x02, CodeSegment, &HandleException0x02, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x03, CodeSegment, &HandleException0x03, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x04, CodeSegment, &HandleException0x04, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x05, CodeSegment, &HandleException0x05, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x06, CodeSegment, &HandleException0x06, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x07, CodeSegment, &HandleException0x07, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x08, CodeSegment, &HandleException0x08, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x09, CodeSegment, &HandleException0x09, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0A, CodeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0B, CodeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0C, CodeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0D, CodeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0E, CodeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0F, CodeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x10, CodeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x11, CodeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x12, CodeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x13, CodeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE);

    // Setup Hardware Interrupt Request (IRQ) handlers remapped starting at hardwareInterruptOffset
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x02, CodeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x03, CodeSegment, &HandleInterruptRequest0x03, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x04,  CodeSegment, &HandleInterruptRequest0x04, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x05, CodeSegment, &HandleInterruptRequest0x05, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x06,  CodeSegment, &HandleInterruptRequest0x06, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x07, CodeSegment, &HandleInterruptRequest0x07, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x08, CodeSegment, &HandleInterruptRequest0x08, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x09, CodeSegment, &HandleInterruptRequest0x09, 0,IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0A, CodeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0B, CodeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0D, CodeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0E, CodeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0F, CodeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE);

    //Initialize Programmable Interrupt Controllers (PIC) ports
    init_port8bit_slow((struct Port8Bit*) & (am->picMasterCommand), 0x20);
    init_port8bit_slow((struct Port8Bit*) &(am->picMasterData), 0x21);
    init_port8bit_slow((struct Port8Bit*) &(am->picSlaveCommand), 0xA0);
    init_port8bit_slow((struct Port8Bit*) &(am->picSlaveData), 0xA1);

    // ICW1: Start PIC initialization in cascade mode
    am->picMasterCommand.Write((struct Port8BitSlow*) & (am->picMasterCommand), 0x11);
    am->picSlaveCommand.Write((struct Port8BitSlow*) & (am->picSlaveCommand), 0x11);

    // ICW2: Remap IRQ base addresses to avoid conflict with CPU exceptions
    am->picMasterData.Write((struct Port8BitSlow*) & (am->picMasterData), hardwareInterruptOffset);
    am->picSlaveData.Write((struct Port8BitSlow*) & (am->picSlaveData), hardwareInterruptOffset + 8);

    // ICW3: Tell Master PIC there is a slave at IRQ2, and tell Slave PIC its identity
    am->picMasterData.Write((struct Port8BitSlow*) & (am->picMasterData), 0x04);
    am->picSlaveData.Write((struct Port8BitSlow*) & (am->picSlaveData), 0x02);

    // ICW4: Set PIC to 8086 mode
    am->picMasterData.Write((struct Port8BitSlow*) & (am->picMasterData), 0x01);
    am->picSlaveData.Write((struct Port8BitSlow*) & (am->picSlaveData), 0x01);

    // OCW1: Unmask all interrupts (0x00 means all enables)
    am->picMasterData.Write((struct Port8BitSlow*) & (am->picMasterData), 0x00);
    am->picSlaveData.Write((struct Port8BitSlow*) & (am->picSlaveData), 0x00);


    // Load the IDT into the CPU register
    idt_pointer.size = 256 * sizeof(struct GateDescriptor) - 1;
    idt_pointer.base = (uint32_t) interruptDescriptorTable;

    asm volatile("lidt %0" :: "m" (idt_pointer));

}

/*
 * Executes 'sti' instruction to start listening for interrupts
 */
void activate_interrupts()
{
    asm volatile ("sti");
}

/*
 * Executes 'cli' instruction to stop listening for interrupts.
 */
void deactivate_interrupts()
{
    asm volatile ("cli");
}

/*
 * Primary C-level handler called by assembly stubs.
 * @param interrupt The interrupt vector number
 * @param esp The stack pointer containing saved registers.
 */
uint32_t HandleInterrupt(uint32_t interrupt, uint32_t esp)
{
    kprintf("Interrupt Occured\n");

    return esp;
}