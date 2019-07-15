#ifndef CLIENT_H
#define CLIENT_H

/*******************************************************************************
* constants
*******************************************************************************/

/* connection */
#define MAX_CLIENTS 10          // max concurrent clients
#define MAX_MSG_LEN 1000        // max message length

#define DEST_ADDR   "127.0.0.1" // server address
#define DEST_PORT   1234        // server port

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
#include <time.h>

/* networking libraries */
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

/* thread libraries */
#include <pthread.h>

/* local libraries */
#include "rw_utils.h"

/*******************************************************************************
* struct
*******************************************************************************/

typedef struct thread_params{
    char dest_addr[16];
    unsigned short dest_port;
    unsigned int thread_id;
} thread_params;

/*******************************************************************************
* functions
*******************************************************************************/

void *sender(void *p);

#endif