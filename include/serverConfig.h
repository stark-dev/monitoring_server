#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

/*******************************************************************************
* constants
*******************************************************************************/

#define CONFIG_FILE_PATH "../config.txt"    // configuration file location
#define MAX_KEY_SIZE 100
#define MAX_VALUE_SIZE 100

#define RET_SUCCESS     0
#define RET_ERROR       1

#define ADDR_SIZE       16

/*******************************************************************************
* libraries
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*******************************************************************************
* struct
*******************************************************************************/

// configuration structure
typedef struct serverConfig {
    unsigned int    max_clients;
    unsigned short  listen_port;
    char            listen_addr[ADDR_SIZE];
} serverConfig;

/*******************************************************************************
* functions
*******************************************************************************/

void init_config(serverConfig *cfg);
int  read_config(serverConfig *cfg);
int  check_pair(serverConfig *cfg, const char *key_p, const char *value_p);

#endif