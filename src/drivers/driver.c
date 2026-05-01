#include <drivers/driver.h>

void init_driver(struct Driver* driver)
{
    driver->Activate = 0;
    driver->Reset = 0;
    driver->Deactivate = 0;
}

void init_driver_manager(struct DriverManager* manager)
{
    manager->numDrivers = 0;

    for (int i = 0; i < MAX_DRIVERS; i++)
        manager->drivers[i] = 0;
}

void driver_manager_add_driver(struct DriverManager* manager, struct Driver* drv)
{
    if (manager->numDrivers < MAX_DRIVERS)
    {
        manager->drivers[manager->numDrivers] = drv;
        manager->numDrivers++;
    }
}

void driver_manager_activate_all(struct DriverManager* manager)
{
    for (int i = 0; i < manager->numDrivers; i++)
        if (manager->drivers[i] != 0 && manager->drivers[i]->Activate != 0)
            manager->drivers[i]->Activate(manager->drivers[i]);
}