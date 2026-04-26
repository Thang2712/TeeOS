#ifndef __TYPE_H
#define __TYPE_H

    /* Signal Integers Types */
    typedef char int8_t;                // extractly 8 bits (1 byte)
    typedef unsigned char uint8_t;      // 8-bit unsigned

    typedef short int16_t;              // extractly 16 bits (2 byte)
    typedef unsigned short uint16_t;    // 16-bit unsigned (common for I/O port)

    typedef int int32_t;                // extractly 32 bits (4 byte)
    typedef unsigned int uint32_t;      // 32-bit unsigned (standard for x86 protected mode)

    typedef long long int64_t;          // extractly 64 bits (8 byte)
    typedef unsigned long long uint64_t;// 64-bit unsigned

    /* Standard definitions */
    /* size_t is used to represent the size of objects in memory.
     * for a 32-bit kernel, uint32_t is the appropriate size.*/
    typedef uint32_t size_t;
#endif
