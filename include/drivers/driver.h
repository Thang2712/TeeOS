#ifndef __TEEOS__DRIVERS__DRIVER_H
#define __TEEOS__DRIVERS__DRIVER_H

    #include <common/types.h>

    // Maximum numbers of the drivers the system can manage at once
    #define MAX_DRIVERS 256

    /*
     * Driver Structure
     * This follows an object-oriented approach in C using pointers.
     */
    typedef struct Driver
    {
        // Called to start the driver's hardware logic.
        void (*Activate)(struct Driver* self);

        // Called to reset the device to a default state. Return 0 on success
        int (*Reset)(struct Driver* self);

        // Called to stop the driver and release hardware resources
        void (*Deactivate)(struct Driver* self);

        /*
         * Pointer to the specific hardware data (e.g., interrupt numbers,
         * port addresses). Equivalent to 'private data' in OOP.
         */
        void* instance;
    } driver_t;

    /*
     * DriverManager Structure
     * Responsible for storing and batch-processing all system drivers.
     */
    typedef struct DriverManager
    {
        // Array of pointers to registered drivers
        struct Driver* drivers[MAX_DRIVERS];

        // Current count of drivers successfully registered
        int numDrivers;
    } driver_manager_t;

    // Function Protopes

    // Initializes the manager, setting numDrivers to 0
    void init_driver_manager(driver_manager_t* manager);

    // Add a new driver to manager's list
    void driver_manager_add_driver(driver_manager_t* manager, driver_t* drv);

    // Iterates through all drivers and calls their Activate() function
    void driver_manager_activate_all(driver_manager_t* manager);

#endif