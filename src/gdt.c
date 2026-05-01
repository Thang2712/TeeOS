#include <gdt.h>

/*
 * set_descriptor: Fill a GDT entry with base, limit, and access flags
 * This version uses a direct struct member access approach.
 */

void set_descriptor(struct SegmentDescriptor *d, uint32_t base, uint32_t limit, uint8_t type)
{
    d->limit_lo = (limit & 0xFFFF);
    d->base_lo = (base & 0xFFFF);
    d->base_hi = (base >> 16) & 0xFF;
    d->type = type;
    d->flags_limit_hi = ((limit >> 16) & 0x0F) | 0xC0 ;
    d->base_vhi = (base >> 24) & 0xFF;
}

/*
 * get_code_segment: Calculates the offset of the Code Segment selector.
 */
uint16_t get_code_segment(struct GlobalDescriptorTable *gdt)
{
    return (uint16_t)((uint8_t*)&gdt->codeSegment - (uint8_t*)gdt);
}

/*
 * get_data_segment: Calculates the offset of the Data Segment selector.
 */
uint16_t get_data_segment(struct GlobalDescriptorTable *gdt)
{
    return (uint16_t)((uint8_t*)&gdt->dataSegment - (uint8_t*)gdt);
}

/*
 * set_segment_descriptor: An alternative setter using byte-array casting.
 * Handles the logic for converting large limits to 4KB page granularity.
 */
void set_segment_descriptor(struct SegmentDescriptor *d, uint32_t base, uint32_t limit, uint8_t type)
{
    uint8_t* target = (uint8_t*) d;

    if (limit < 65536)
        // small limit: 16-bit address space, byte granularity
        target[6] = 0x40;
    else 
    {
        // large limit: use 4kb page granularity (g-bit = 1)
        if ((limit & 0xFFF) != 0xFFF)
            limit = (limit >> 12) - 1;
        else
            limit = (limit >> 12);
        target[6] = 0xC0;       // 0xC0 = 1100b (granularity + Size bits)
    }

    // Encode the limit into bytes 0, 1, and part of 6
    target[0] = limit & 0xFF;
    target[1] = (limit >> 8) & 0xFF;
    target[6] |= (limit >> 16) & 0xF;

    // Encode the base address into bytes 2, 3, 4, and 7.
    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;

    // Byte 5 is the Access Rights/Type byte.
    target[5] = type;


}

/*
 * get_segment_base: Extracts the 32-bits base address from a descriptor
 */
uint16_t get_segment_base(struct SegmentDescriptor *d) 
{
    uint8_t *target = (uint8_t*)d;
    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];

    return result;
}

/*
 * get_segment_limits: Extracts the segment limit from a descriptor
 */
uint32_t get_segment_limit(struct SegmentDescriptor *d) 
{
    uint8_t *target = (uint8_t*)d;
    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];
    return result;
}

/*
 * init_gdt: Initializes the GDT with NULL, Code and Data Segment.
 * then informs the CPU of its location using 'ldgt' instruction.
 */
void init_gdt(struct GlobalDescriptorTable *gdt)
{
    // Setup individual segments
    set_segment_descriptor(&gdt->nullSegment, 0, 0, 0);     // NULL segment(required)
    set_segment_descriptor(&gdt->unused, 0, 0, 0);          // Placeholder
    set_segment_descriptor(&gdt->codeSegment, 0, 0xFFFFFFFF, 0x9A);     // Code: Base 0, Limit 4Gb, Exec/read
    set_segment_descriptor(&gdt->dataSegment, 0, 0xFFFFFFFF, 0x92);     // Data: Base 0, limit 4gb, Read/write


    // Prepare the GDT Pointer (GDTR register format)
    struct 
    {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) gdt_ptr;

    gdt_ptr.limit = sizeof(struct GlobalDescriptorTable) - 1;       // limit is size - 1
    gdt_ptr.base = (uint32_t)gdt;                                   // linear address of gdt

    // Load the GDT into the CPU register
    asm volatile ("lgdt %0" : : "m" (gdt_ptr));
}