#ifndef MAIN_H
#define MAIN_H

/*******************************************************************************
* libraries
*******************************************************************************/

/* C standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

/* networking libraries */
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

/* file handling libraries */
#include <sys/stat.h>
#include <sys/types.h>

/* system libraries */
#include <syslog.h>

/* local includes */
#include "rw_utils.h"
#include "serverConfig.h"

/*******************************************************************************
* constants
*******************************************************************************/

/* system */
#define CONSOLE_LOG 1           // enable console log

/* connection */
#define SELECT_TIMEOUT_SEC 20   // listener select timeout (avoid being stuck if no client connects)

#define MAX_INCOMING 3          // max concurrent client requests handled
#define MAX_MSG_LEN 1000        // max message length

/* devices */
#define MAX_DEV_NAME_LEN 100    // max device name len accepted

/* logger */
#define MAX_LOG_FILE_LEN    MAX_LOG_FOLDER_LEN + MAX_DEV_NAME_LEN + 5

/*******************************************************************************
* functions
*******************************************************************************/

int create_log_folder(const char *name);

void set_log_level(int level);

void open_syslog(const char *name);
void close_syslog();

#endif