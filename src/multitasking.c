#include <multitasking.h>

// Task Implementation
void task_init(task_t* task, void (*entrypoint)(), uint32_t code_segment)
{
    task->cpustate = (cpu_state_t*)(task->stack + 4096 - sizeof(cpu_state_t));
    
    task->cpustate->eax = 0;
    task->cpustate->ebx = 0;
    task->cpustate->ecx = 0;
    task->cpustate->edx = 0;

    task->cpustate->esi = 0;
    task->cpustate->edi = 0;
    task->cpustate->ebp = 0;
    
    uint32_t dataSelector = 0x10; 
    task->cpustate->ds = dataSelector;
    task->cpustate->es = dataSelector;
    task->cpustate->fs = dataSelector;
    task->cpustate->gs = dataSelector;
    
    task->cpustate->eip = (uint32_t)entrypoint;
    task->cpustate->cs = code_segment;
    
    task->cpustate->eflags = 0x202;
    
    task->cpustate->error_code = 0;
    task->cpustate->interrupt_number = 0;
}

// Task Manager Implementation
void task_manager_init(task_manager_t* tm)
{
    tm->numTasks = 0;
    tm->currentTask = -1;
}

bool task_manager_add_task(task_manager_t* tm, task_t* task)
{
    if (tm->numTasks >= 256)
        return false;
        
    tm->tasks[tm->numTasks++] = task;
    return true;
}

cpu_state_t* task_manager_schedule(task_manager_t* tm, cpu_state_t* cpustate)
{
    if (tm->numTasks <= 0)
        return cpustate;
    
    if (tm->currentTask >= 0)
        tm->tasks[tm->currentTask]->cpustate = cpustate;
    
    if (++tm->currentTask >= tm->numTasks)
        tm->currentTask %= tm->numTasks;
        
    return tm->tasks[tm->currentTask]->cpustate;
}