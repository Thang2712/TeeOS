/*
 * ENTRY: Specific the symbol that marks the begining of execution
 * This corresponds to the 'loader' label in my loader.s file
 */
ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    /*
     * The '.' (location counter) starts at 1MB (0x100000)
     * This is the conventional load address for Multiboot kernels to avoid
     * conflicting with lower memory regions used by BIOS and VGA
     */
    . = 1M;

    /*
     * .text: The section containing the executable code
     */
    .text :
    {
        /*
         * KEEP: Ensures the Multiboot header is placed at the very
         * begining of the binary. If it's not within the first 8KB,
         * GRUB/QEMU will fail to find it
         */
        KEEP(*(.multiboot))

        /*
         * Include all other code from all input files
         */
        *(.text)
    }

    /*
     * .data: Section for initialized global/static variables
     */
    .data : { *(.data) }

    /*
     * .bss: Section for uninitialized global variables and my stack
     */
    .bss  : { *(.bss)  }

    /Discard/  :
    {
        *(.fin_array*)
        *(.comment)
    }
}