#Mapping IRQs to start at  0x20 to avoid conflict with CPU exceptions (0x00 - 0x1F)
.set IRQ_BASE, 0x20

.section .text

# High-level C handler function defined in interrupts.c
.extern HandleInterrupt

# Macro for Exceptions that do not push an error onto the stack by defaults
# We push a dummy '0' to keep the stack fram consistent
.macro HandleExceptionNoErrorCode num
.global HandleException\num
HandleException\num:
    pushl $0            # Dummy error code
    pushl $\num         # Interrupt number
    jmp int_bottom
.endm

# Macro for Exceptions that already push an error code (like Page fault or Double Fault)
.macro HandleExceptionWithErrorCode num
.global HandleException\num
HandleException\num:
    pushl $\num             # Interrupt number (Error code is already pushed by CPU)
    jmp int_bottom
.endm

# Macro for Hardware Interrupt Requests (IRQs) from PIC
.macro HandleInterruptRequest num
.global HandleInterruptRequest\num
HandleInterruptRequest\num:
    pushl $0                        # Dummy error code
    pushl $(\num + IRQ_BASE)        # Remapped interrupt number
    jmp int_bottom
.endm

# CPU Exceptions (0x00 - 0x13)
HandleExceptionNoErrorCode 0x00         # Division by zero
HandleExceptionNoErrorCode 0x01         # Debug
HandleExceptionNoErrorCode 0x02         # Non-maskable Interrupt
HandleExceptionNoErrorCode 0x03         # Breakpoint
HandleExceptionNoErrorCode 0x04         # Overflow
HandleExceptionNoErrorCode 0x05         # Bound Range Exceed
HandleExceptionNoErrorCode 0x06         # Invalid Opcode
HandleExceptionNoErrorCode 0x07         # Device Not Available

HandleExceptionWithErrorCode 0x08       # Double Fault (Has Error Code)

HandleExceptionNoErrorCode 0x09         # Coprocessor Segment Overrun

HandleExceptionWithErrorCode 0x0A       # Invalid TSS
HandleExceptionWithErrorCode 0x0B       # Segment Not Present
HandleExceptionWithErrorCode 0x0C       # Stack-Segment Fault
HandleExceptionWithErrorCode 0x0D       # General Protection Fault
HandleExceptionWithErrorCode 0x0E       # Page Fault

HandleExceptionNoErrorCode 0x0F         # Reserved
HandleExceptionNoErrorCode 0x10         # x87 Floating-Point Exception

HandleExceptionWithErrorCode 0x11       # Alignment Check

HandleExceptionNoErrorCode 0x12         # Machine Check
HandleExceptionNoErrorCode 0x13         # SIMD Floating-Point Exception

# Hardware IRQs (Master and Slave PIC)
HandleInterruptRequest 0x00             # IRQ0: Timer
HandleInterruptRequest 0x01             # IRQ1: Keyboard
HandleInterruptRequest 0x02             # IRQ2: Cascade (Slave PIC)
HandleInterruptRequest 0x03             # IRQ3: COM2
HandleInterruptRequest 0x04             # IRQ4: COM1
HandleInterruptRequest 0x05             # IRQ5: LPT2
HandleInterruptRequest 0x06             # IRQ6: Floppy Disk
HandleInterruptRequest 0x07             # IRQ7: LPT1
HandleInterruptRequest 0x08             # IRQ8: CMOS RTC
HandleInterruptRequest 0x09             # IRQ9: Free
HandleInterruptRequest 0x0A             # IRQ10: Free
HandleInterruptRequest 0x0B             # IRQ11: Free
HandleInterruptRequest 0x0C             # IRQ12: PS2 Mouse
HandleInterruptRequest 0x0D             # IRQ13: FPU
HandleInterruptRequest 0x0E             # IRQ14: Primary ATA
HandleInterruptRequest 0x0F             # IRQ15: Secondary ATA

# Common entry for all interrupts to save CPU state
int_bottom:
    #1. Save all general purpose registers
    pusha

    #2. Save segment registers (data, extra, f, g)
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    #3. Push the current Stack Pointer as a parameter for the C function
    pushl %esp
    call HandleInterrupt

    #4. The return value in %eax is the updated stack pointer - use it
    mov %eax, %esp

    #5. Restore segment registers
    popl %gs
    popl %fs
    popl %es
    popl %ds

    #6. Restore general purpose registers
    popa
    #7. Clean up the interrupt number and error code pushed by the macros
    addl $8, %esp

    #8. Return from interrupt (Restores CS, EIP, EFLAGS, etc.)
    iret

# A "safe" handler that does nothing for unmapped interrupts
.global InterruptIgnore
InterruptIgnore:
    iret
