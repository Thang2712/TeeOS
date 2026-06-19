
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

    #include <common/types.h>
    #include <gdt.h>
    #include <stdbool.h>

    typedef struct
    {
        uint32_t gs; 
        uint32_t fs;
        uint32_t es;
        uint32_t ds; 

        uint32_t esp_dummy; 
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
        uint32_t esi;
        uint32_t edi;
        uint32_t ebp;

        uint32_t error_code;
        uint32_t interrupt_number;

        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
        uint32_t esp;
        uint32_t ss;
    } __attribute__((packed)) cpu_state_t;

    typedef struct
    {
        uint8_t stack[4096];
        cpu_state_t* cpustate;
    } task_t;

    typedef struct
    {
        task_t* tasks[256];
        int numTasks;
        int currentTask;
    } task_manager_t;

    void task_init(task_t* task, void (*entrypoint)(), uint32_t code_segment);

    // TaskManager API
    void task_manager_init(task_manager_t* tm);
    bool task_manager_add_task(task_manager_t* tm, task_t* task);
    cpu_state_t* task_manager_schedule(task_manager_t* tm, cpu_state_t* cpustate);
#endif
