#ifndef __TEEOS__DRIVERS__DRIVER_H
#define __TEEOS__DRIVERS__DRIVER_H

    #include <common/types.h>

    #define MAX_DRIVERS 256

    typedef struct Driver
    {
        void (*Activate)(struct Driver* self);
        int (*Reset)(struct Driver* self);
        void (*Deactivate)(struct Driver* self);

        void* instance;
    } driver_t;

    typedef struct DriverManager
    {
        struct Driver* drivers[MAX_DRIVERS];
        int numDrivers;
    } driver_manager_t;

    void init_driver_manager(driver_manager_t* manager);
    void driver_manager_add_driver(driver_manager_t* manager, driver_t* drv);
    void driver_manager_activate_all(driver_manager_t* manager);

#endif