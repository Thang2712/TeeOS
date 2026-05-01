
#ifndef __TEEOS__HARDWARECOMMUNICATION__PORT_H
#define __TEEOS__HARDWARECOMMUNICATION__PORT_H

    #include <common/types.h>

    /*
     * @brief Basic Port structure.
     * Represents a generic hardware port address
     */
    typedef struct Port
    {
        uint16_t port;
    } port_t;

    /*
     * @brief 8-bit I/O port.
     * Used for devices that exchange data 1 byte at a time (e.g., PS/2 Keyboard, Serial Port)
     */
    typedef struct Port8Bit
    {
        uint16_t portnumber;    // The 16-bit address of I/O port

        // Function pointer to write an 8-bit byte to the hardware port
        void (*Write) (struct Port8Bit *self, uint8_t data);

        // Function pointer to read an 8-bit byte from the hardware port
        uint8_t (*Read) (struct Port8Bit *self);
    } port8bit_t;

    /*
     * @brief 8-bit I/O port with Small Delay (slow)
     * Used for older hardware that requires a small pause after writing to
     * allow the electronics to catch up (e.g., CMOS, PIT, or older DMA controllers).
     */
    typedef struct Port8BitSlow
    {
        uint16_t portnumber;
        // Function pointer to write an 8-bit byte with an I/O wait operation
        void (*Write) (struct Port8BitSlow *self, uint8_t data);
    } port8bitslow_t;

    /*
     * @brief 16-bit I/O port
     * Used for devices exchanging 2-byte words (e.g., IDE hard drive controllers)
     */
    typedef struct Port16Bit
    {
        uint16_t portnumber;
        // Function pointer to write a 16-bit word to the hardware port
        void (*Write) (struct Port16Bit *self, uint16_t data);
        // Function pointer to read a 16-bit word from the hardware port
        uint16_t (*Read) (struct Port16Bit *self);
    } port16bit_t;

    /*
     * @brief 32-bit I/O port
     * Used for modern high-bandwidth devices like PCI controllers or some netword cards.
     */
    typedef struct Port32Bit
    {
        uint16_t portnumber;
        // Function pointer to write a 32-bit double-word to the hardware port
        void (*Write) (struct Port32Bit *self, uint32_t data);
        // Function pointer to read a 32-bit double-word from the hardware port
        uint32_t (*Read) (struct Port32Bit *self);
    } port32bit_t;

    // Initialization Functions

    // @brief Assigns the port number and link the Read/Write assembly wrappers for 8-bits ports
    void init_port8bit(port8bit_t *self, uint16_t data);
    // @brief Assigns the port number and links the slow Write assembly wrapper
    void init_port8bit_slow(port8bitslow_t *self, uint16_t data);
    // @brief Assigns the port number and link the Read/Write assembly wrapper for 16-bit ports
    void init_port16bit(port16bit_t *self, uint16_t data);
    // @brief Assigns the port number and links the Read/Write assembly wrappers for 32-bit ports
    void init_port32bit(port32bit_t *self, uint32_t data);

#endif