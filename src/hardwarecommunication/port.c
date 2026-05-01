#include <hardwarecommunication/port.h>

/* Hardware abstraction layer (inline assembly) */

/*
 * outb: sends a 8-bit byte to a specific I/O port
 * @param port: the 16-bit address of the hardware port
 * @param data: the 8-bit value to send
 */
static inline void outb(uint16_t port, uint8_t data) 
{
    // "a" (data) puts data in EAX, "Nd" (port) puts port in a constant or DX
    asm volatile ("outb %0, %1" : : "a" (data), "Nd"(port));
}

/*
 * inb: reads an 8-bit byte from specific I/O port
 * @return: the byte read from the hardware
 */
static inline uint8_t inb(uint16_t port) 
{
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

/*
 * outb_show: sends data with small delay.
 * useful for older hardware (like the PIC) that cannot press commands
 * at the full of speed of the modern CPU.
 */
static inline void outb_show(uint16_t port, uint8_t data) 
{
    asm volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (port));
}

/* 8-Bit Port Implementation */
void port8bit_write(port8bit_t* self, uint8_t data) 
{
    outb(self->portnumber, data);
}

uint8_t port8bit_read(port8bit_t* self) 
{
    return inb(self->portnumber);
}

void port8bit_slow_write(port8bitslow_t* self, uint8_t data) 
{
    outb_show(self->portnumber, data);
}

/* 16-Bit Port Implementation */
void port16bit_write(port16bit_t* self, uint16_t data) 
{
    // outw is used for 16-bit (word) data transfers
    asm volatile ("outw %0, %1" : : "a" (data), "Nd"(self->portnumber));
}

uint16_t port16bit_read(port16bit_t* self) 
{
    uint16_t result;
    asm volatile ("inw %1, %0" : "=a"(result) : "Nd"(self->portnumber));
    return result;
}

/* 32-Bit Port Implemtation */
void port32bit_write(port32bit_t* self, uint32_t data) 
{
    // outl is used for 32-bit (long) data transfers
    asm volatile ("outl %0, %1" : : "a" (data), "Nd" (self->portnumber));
}

uint32_t port32bit_read(port32bit_t* self) 
{
    uint32_t result;
    asm volatile ("inl %1, %0" : "=a" (result) : "Nd"(self->portnumber));
    return result;
}

/* Constructor Function */

/*
 * these function initialize the struct by assigning the port number
 * and linking the function to the actual implementations
 */
void init_port8bit(port8bit_t *self, uint16_t portnumber) 
{
    self->portnumber = portnumber;
    self->Write = port8bit_write;
    self->Read = port8bit_read;
}

void init_port8bit_slow(port8bitslow_t *self, uint16_t portnumber) 
{
    self->portnumber = portnumber;
    self->Write = port8bit_slow_write;
    // Note: no read function is typically used for slow parts
}

void init_port16bit(port16bit_t *self, uint16_t portnumber) 
{
    self->portnumber = portnumber;
    self->Write = port16bit_write;
    self->Read = port16bit_read;
}

void init_port32bit(port32bit_t *self, uint32_t portnumber) 
{
    self->portnumber = portnumber;
    self->Write = port32bit_write;
    self->Read = port32bit_read;
}