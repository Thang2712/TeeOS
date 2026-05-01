
#ifndef __TEEOS__HARDWARECOMMUNICATION__PORT_H
#define __TEEOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

typedef struct Port
{
    uint16_t port;
} port_t;

typedef struct Port8Bit
{
    uint16_t portnumber;
    void (*Write) (struct Port8Bit *self, uint8_t data);
    uint8_t (*Read) (struct Port8Bit *self);
} port8bit_t;

typedef struct Port8BitSlow
{
    uint16_t portnumber;
    void (*Write) (struct Port8BitSlow *self, uint8_t data);
} port8bitslow_t;

typedef struct Port16Bit
{
    uint16_t portnumber;
    void (*Write) (struct Port16Bit *self, uint16_t data);
    uint16_t (*Read) (struct Port16Bit *self);
} port16bit_t;

typedef struct Port32Bit
{
    uint16_t portnumber;
    void (*Write) (struct Port32Bit *self, uint32_t data);
    uint32_t (*Read) (struct Port32Bit *self);
} port32bit_t;

void init_port8bit(port8bit_t *self, uint16_t data);
void init_port8bit_slow(port8bitslow_t *self, uint16_t data);
void init_port16bit(port16bit_t *self, uint16_t data);
void init_port32bit(port32bit_t *self, uint32_t data);

#endif