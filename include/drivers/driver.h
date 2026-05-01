#ifndef __TEEOS__DRIVERS__DRIVER_H
#define __TEEOS__DRIVERS__DRIVER_H

    #include <common/types.h>

    #define MAX_DRIVERS 256

    struct Driver
    {
        void (*Activate)(struct Driver* self);
        int (*Reset)(struct Driver* self);
        void (*Deactivate)(struct Driver* self);

        void* instance;
    };

    struct DriverManager
    {
        struct Driver* drivers[MAX_DRIVERS];
        int numDrivers;
    };

    void init_driver_manager(struct DriverManager* manager);
    void driver_manager_add_driver(struct DriverManager* manager, struct Driver* drv);
    void driver_manager_activate_all(struct DriverManager* manager);

#endif