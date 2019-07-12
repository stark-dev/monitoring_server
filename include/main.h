#ifndef MAIN_H
#define MAIN_H

/*******************************************************************************
* constants
*******************************************************************************/

/* connection */
#define LISTEN_IP "0.0.0.0"     // server listen IP address (all)
#define LISTEN_PORT 1234        // server listen port
#define SELECT_TIMEOUT_SEC 20   // listener select timeout (avoid being stuck if no client connects)

#define MAX_CLIENTS 10          // max client connections allowed
#define MAX_INCOMING 3          // max concurrent client requests handled
#define MAX_MSG_LEN 1000        // max message length

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


#endif