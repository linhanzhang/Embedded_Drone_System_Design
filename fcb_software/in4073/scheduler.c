/*------------------------------------------------------------------
 *  scheduler.c
 *
 *  Koen Wösten
 *
 *  11 May 2022
 *------------------------------------------------------------------
 */

#include "scheduler.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "app_util_platform.h"
#include "timers.h"
#include "fail.h"

static task *tasks[SCHEDULER_MAX_TASKS];
static uint8_t cur_tasks = 0;
static uint16_t failed = 0;

/**
 * @brief the to-be hooked function by a timer.
 *
 * @author Koen Wösten
 */
void TimerISR(void)
{
    sd_nvic_critical_region_enter(0);

    for (int i = 0; i < cur_tasks; ++i)
    {
        if (tasks[i]->elapsedTime >= tasks[i]->period)
        { // Ready
            tasks[i]->elapsedTime = 0;
            tasks[i]->pending++;
        }
        tasks[i]->elapsedTime += TIMER_PERIOD;
    }
    clear_timer_flag();

    sd_nvic_critical_region_exit(0);

    return;
}

/**
 * @brief Returns the amount of times the execution was over time
 *
 * @author Koen Wösten
 *
 * @param task task to add
 */
uint16_t scheduler_n_failed(void)
{
    return failed;
}

/**
 * @brief reset the scheduler, removes all pending tasks
 *
 * @author Koen Wösten
 *
 */
void scheduler_reset(void)
{
    for (int i = 0; i < cur_tasks; i++)
    {
        tasks[i]->pending = 0;
    }
}

/**
 * @brief Adds a task to the task-list.
 *
 * @author Koen Wösten
 *
 * @param task task to add
 */
void scheduler_add_task(task *task)
{
    if (strlen(task->name) > SCHEDULER_MAX_NAME_LENGTH)
    {
#ifdef DEBUG
        printf("[sched] task name too long\n");
#endif
        return;
    }
    tasks[cur_tasks] = task;
    cur_tasks++;
}

/**
 * @brief get a task from the task list.
 *
 * @param name name identifier of the task to get
 *
 * @author Koen Wösten
 */
task *scheduler_get_task(char *name)
{
    int i;
    for (i = 0; i < cur_tasks; i++)
    {
        if (strncmp(tasks[i]->name, name, SCHEDULER_MAX_NAME_LENGTH) == 0)
        {
            return tasks[i];
        }
    }
    return NULL;
}

/**
 * @brief Removes a task from the task list.
 *
 * @author Koen Wösten
 *
 * @param name name identifier of the task to remove
 */
void scheduler_remove_task(char *name)
{
    int i;
    for (i = 0; i < cur_tasks; i++)
    {
        if (strncmp(tasks[i]->name, name, SCHEDULER_MAX_NAME_LENGTH) == 0)
        {
            break;
        }
    }

    for (; i < cur_tasks - 1; i++)
    {
        tasks[i] = tasks[i + 1];
    }

    // tasks[cur_tasks-1] = 0;
    cur_tasks--;
}

/**
 * @brief Simple scheduler function, runs all ready tasks and updates their elapsed time.
 *
 * @author Koen Wösten
 *
 * @return * void
 */
void scheduler()
{
    for (int i = 0; i < cur_tasks; ++i)
    {
        int exec = 0;
        sd_nvic_critical_region_enter(0);
        if (tasks[i]->pending)
        { // Ready
            tasks[i]->pending--;
            exec = 1;
        }
        sd_nvic_critical_region_exit(0);

        if (exec)
        {
            uint32_t before = get_time_us();
            tasks[i]->taskFunc();

            sd_nvic_critical_region_enter(0);
            
            uint32_t diff = get_time_us() - before;
            tasks[i]->execTime = diff;

            sd_nvic_critical_region_exit(0);
        }
    }
    clear_timer_flag();
}
