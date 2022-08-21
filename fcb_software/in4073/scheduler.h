/*------------------------------------------------------------------
 *  scheduler.h -- Prototypes and definitions for the scheduler
 *
 *  Koen Wösten
 *  
 *  11 May 2022
 *------------------------------------------------------------------
 */


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <inttypes.h>

#define SCHEDULER_MAX_TASKS 8
#define SCHEDULER_MAX_NAME_LENGTH 32

typedef void task_fn(void);

/**
 * @brief periodic task struct
 * 
 * @author Koen Wosten
 */
typedef struct task {
   char* name;                  //name of the task
   uint32_t period;             // Rate at which the task should tick
   uint32_t elapsedTime;        // Time since task's last tick
   task_fn *taskFunc;           // Function to call for task's tick
   uint32_t pending;            // Numbers of tasks of this type is pending
   uint32_t execTime;           // Latest execution time. (Start to finish) 
} task;

void TimerISR(void);
void scheduler_add_task(task* task);
void scheduler_remove_task(char* name);
task* scheduler_get_task(char* name);
void scheduler(void);
void scheduler_reset(void);
uint16_t scheduler_n_failed(void);


#endif /* SCHEDULER_H_ */ 