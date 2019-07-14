#ifndef MAIN_H
#define MAIN_H

/*******************************************************************************
* constants
*******************************************************************************/

/* connection */
#define SELECT_TIMEOUT_SEC 20   // listener select timeout (avoid being stuck if no client connects)

#define MAX_INCOMING 3          // max concurrent client requests handled
#define MAX_MSG_LEN 1000        // max message length

#define THREAD_COUNT    4 

/* devices */
#define MAX_DEV_NAME_LEN 100    // max device name len accepted


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

/* networking libraries */
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

/* thread libraries */
#include <pthread.h>

/* local includes */
#include "serverConfig.h"

#endif

/*******************************************************************************
* functions
*******************************************************************************/

void *connection_handler();