#include <drivers/driver.h>

/*
 * @brief Resets a driver structure to its default state.
 * Sets all function pointers (Activate, Reset, Deactivate) to null (0)
 */
void init_driver(driver_t* driver)
{
    driver->Activate = 0;
    driver->Reset = 0;
    driver->Deactivate = 0;
}

/*
 * @brief Initializes the Driver Manager.
 * Set the driver count to zero and clears the driver pointers array
 * to prevent garbage value from causing system crashes
 */
void init_driver_manager(driver_manager_t* manager)
{
    manager->numDrivers = 0;

    for (int i = 0; i < MAX_DRIVERS; i++)
        manager->drivers[i] = 0;
}

/*
 * @brief Registers a new driver into the manager.
 * Adds the driver to the internal list if there is remaining space.
 * @param manager Pointer to the system's driver maanger.
 * @param drv Pointer to the driver structure to be added
 */
void driver_manager_add_driver(driver_manager_t* manager, driver_t* drv)
{
    if (manager->numDrivers < MAX_DRIVERS)
    {
        manager->drivers[manager->numDrivers] = drv;
        manager->numDrivers++;
    }
}

/*
 * @brief Activates all registered drivers in the system
 * Iterates through the list and excutes the Activate() function pointer
 * for each driver, provided the driver and the function pointer are valid.
 */
void driver_manager_activate_all(driver_manager_t* manager)
{
    for (int i = 0; i < manager->numDrivers; i++)
        // Safety check: ensure driver exists and has an activation method
        if (manager->drivers[i] != 0 && manager->drivers[i]->Activate != 0)
            // Call the function pointer, passing the driver itself as 'self'
            manager->drivers[i]->Activate(manager->drivers[i]);
}