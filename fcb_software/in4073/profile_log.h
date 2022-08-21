/*------------------------------------------------------------
 *profile_log.h - Profiling and Logging for Drone

 * Author - Siyuan Fang
 *
 * May 2022
 *------------------------------------------------------------
 */

#ifndef PROFILE_LOG_H
#define PROFILE_LOG_H

#include <stdbool.h>
#include <stdint.h>

#define LOGGING_SIZE 31

void log_init();
bool log_write();
void log_read();

#endif