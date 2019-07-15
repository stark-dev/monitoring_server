#ifndef LOGGER_H
#define LOGGER_H

/*******************************************************************************
* libraries
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>

/*******************************************************************************
* libraries
*******************************************************************************/

#define CONSOLE_LOG_EN  1
#define SYS_LOG_EN      1

/*******************************************************************************
* functions
*******************************************************************************/

void set_log_level(int level);

void open_syslog(const char *name);
void close_syslog();

void user_log(int level, const char* fmt, ...);

#endif