#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

/*******************************************************************************
* libraries
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

/*******************************************************************************
* constants
*******************************************************************************/

/* config file */
#define CONFIG_FILE_PATH "../config.txt"    // configuration file location
#define MAX_KEY_SIZE 100
#define MAX_VALUE_SIZE 100

/* return value */
#define RET_SUCCESS     0
#define RET_ERROR       1

/* options limits */
#define MAX_CLIENTS     128                 // maximum number of clients accepted

/* ip address */
#define ADDR_SIZE       16

/* logger */
#define MAX_LOG_FOLDER_LEN 100              // max log folder name len accepted

/*******************************************************************************
* struct
*******************************************************************************/

// configuration structure
typedef struct serverConfig {
    unsigned int    max_clients;
    unsigned short  listen_port;
    char            listen_addr[ADDR_SIZE];
    char            log_folder[MAX_LOG_FOLDER_LEN];
    unsigned short  log_level;
} serverConfig;

/*******************************************************************************
* functions
*******************************************************************************/

void init_config(serverConfig *cfg);
int  read_config(serverConfig *cfg);
int  check_pair(serverConfig *cfg, const char *key_p, const char *value_p);

#endif