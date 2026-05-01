#Magic: A specific number that tells the bootloder "this is multiboot kernel"
.set MAGIC,    0x1BADB002

#Flags: Configuration for the bootloaders.
# (1 << 0) - Align all boot modules on i386 page boundaries.
# (1 << 1) - Provide a memory map (info about available RAM)
.set FLAGS,    (1 << 0) | (1 << 1)

#CHECKSUM: Must be -(MAGIC + FLAGS). The bootloaders adds all three value.
#if the result is 0, the header is considered valid.
.set CHECKSUM, -(MAGIC + FLAGS)

#Multiboot header
#this section must be within the first 8KB of the kernel file.
.section .multiboot
    .align 4        #header must be 32-bits(4-byte) aligned
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

#CodeSection
.section .text
.extern kernelMain      #declared that 'kernelMain' is defined in another file (kernel.c)
.global loader          #the entry point for the linker

loader:
    #Initialize the Stack Pointer (esp)
    #x86 stacks grow downward, so we point %esp to the end of reserved memory
    mov $kernel_stack, %esp

    #Jump to the C entry point. We should never return from this
    call kernelMain
#Fallback loop: if kernelMain returns or fails, we halt the CPU
_stop:
    cli         #Disable interupts
    hlt         #Halt the CPU (wait for the next interupt/event)
    jmp _stop   #Infinity loop for a safety measure

#DataSection
.section .bss
.align 16       #Align the stack for performance/compatibility (16-byte)
    .skip 16384 #Reverse 16 KiB of space for the kernel stack

kernel_stack:   #This Label points to the BOTTOM (highest address) of the stack
