
#ifndef __TEEOS__GDT_H
#define __TEEOS__GDT_H

    #include <common/types.h>

    /*
     * Structure representing a GDT Segment Descriptor (8 bytes)
     * It defines the properties of a memory segment, such as its size
     * location, and access right.
     */

    struct SegmentDescriptor
    {
        uint16_t limit_lo;              // Lower 16 bits of the segment limit(size)
        uint16_t base_lo;               // Lower 16 bits of the base address
        uint8_t base_hi;                // Middle 8 bits (bits 16-23) of the base address
        uint8_t type;                   // Access rights and segment type (e.g. Code/Data, Privilege Level)
        uint8_t flags_limit_hi;         // Upper 4 bits of the limit and 4 bits of flags (Granularity, Size)
        uint8_t base_vhi;               // Final 8 bits (bits 24 - 31) of the base address
    } __attribute__((__packed__));

    /*
     *  Structure representing the Global Descriptor Table (GDT) layout
     *  This basic setup typically includes a NULL segment, followed by Code and Data Segment
     */
    struct GlobalDescriptorTable
    {
        struct SegmentDescriptor nullSegment;       // Mandatory NULL descriptor (all zeros)
        struct SegmentDescriptor unused;            // Optional unused/ extra segment
        struct SegmentDescriptor codeSegment;       // Descriptor for the Kernel Code Segment
        struct SegmentDescriptor dataSegment;       // Descriptor for the Kernel Data Segment
    } __attribute__((__packed__));

    /*
     *  Initializes an individual segment descriptor with the given parameters
     *  @param d Pointer to the descriptor to initialize
     *  @param base The 32-bit starting address of the segment
     *  @param limit The size of the segment
     *  @param type Access rights and type flags for the segment
     */
    void init_gdt_descriptor(struct SegmentDescriptor* d, uint32_t base, uint32_t limit, uint8_t type);

    /*
     * Loads the GDT into the CPU's GDTR register using the LGDT assembly instruction
     * @param gdt Pointer to the populated Global Descriptor Table structure.
     */
    void setup_gdt(struct GlobalDescriptorTable* gdt);


    /*
     * Calculates and returns the offset (selector) for the Code Segment.
     * Usually used to set the CS register
     */
    uint16_t get_code_selector(struct GlobalDescriptorTable* gdt);

    /*
     * Calculates and returns the offset (selector) for the Data Segment
     * Usually used to set DS, ES, FS, GS, and SS registers.
     */
    uint16_t get_data_selector(struct GlobalDescriptorTable* gdt);

    /*
     * The main initialization function that fills the GDT entries and activates it.
     * Typically called at the very beginning of kernel execution.
     */
    void init_gdt(struct GlobalDescriptorTable* gdt);

#endif