
#ifndef __PORT_H
#define __PORT_H

#include "types.h"

struct Port
{
    uint16_t port;
};

struct Port8Bit
{
    uint16_t portnumber;
    void (*Write) (struct Port8Bit *self, uint8_t data);
    uint8_t (*Read) (struct Port8Bit *self);
};

struct Port8BitSlow
{
    uint16_t portnumber;
    void (*Write) (struct Port8BitSlow *self, uint8_t data);
};

struct Port16Bit
{
    uint16_t portnumber;
    void (*Write) (struct Port16Bit *self, uint16_t data);
    uint16_t (*Read) (struct Port16Bit *self);
};

struct Port32Bit
{
    uint16_t portnumber;
    void (*Write) (struct Port32Bit *self, uint32_t data);
    uint32_t (*Read) (struct Port32Bit *self);
};

void init_port8bit(struct Port8Bit *self, uint16_t data);
void init_port8bit_slow(struct Port8Bit *self, uint16_t data);
void init_port16bit(struct Port16Bit *self, uint16_t data);
void init_port32bit(struct Port32Bit *self, uint32_t data);

#endif