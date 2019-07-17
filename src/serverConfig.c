#include "../include/serverConfig.h"

/*******************************************************************************
* function definitions
*******************************************************************************/

void init_config(serverConfig *cfg) {
    cfg->max_clients = 10;
    cfg->listen_port = 1234;
    strncpy(cfg->listen_addr, "0.0.0.0", ADDR_SIZE - 1);
    strncpy(cfg->log_folder, "log", MAX_LOG_FOLDER_LEN - 1);
    cfg->log_level = LOG_DEBUG;

    return;
}

int read_config(serverConfig *cfg) {

    /****************************** declarations ******************************/

    FILE *cfg_fp;                   // file pointer

    char *buffer = NULL;            // read buffer (will be allocated by getline)
    size_t len = 0;                 // len set to 0 tells getline to allocate buffer
    ssize_t rd_count;               // ret value of getline (-1  error or EOF)
    unsigned int line_count;        // config file line count

    char *key_p;                    // key pointer
    char *value_p;                  // value pointer

    int ret_value = RET_SUCCESS;    // return value (0 -> ok, 1 -> error)

    // tokenize
    char *ptr = NULL;
    char delim[] = " \n\t";        // split on whitespace, tab and new line

    // local configuration
    serverConfig local_cfg;

    /**************************************************************************/

    // load default config before reading from file (avoid unknown behavior if config file is corrupted)
    init_config(cfg);
    memcpy(&local_cfg, cfg, sizeof(serverConfig));

    syslog(LOG_INFO, "Reading configuration from config file %s\n", CONFIG_FILE_PATH);

    // open config file
    cfg_fp = fopen(CONFIG_FILE_PATH, "r");

    if(cfg_fp == NULL) {       // file error
        syslog(LOG_ERR, "Error reading configuration file: %s\n", strerror(errno));
        ret_value = RET_ERROR;
    }
    else {
        line_count = 1;

        while((rd_count = getline(&buffer, &len, cfg_fp)) != -1) {
            
            ptr = strtok(buffer, delim);    // split line

            // first chunk: read key
            if(ptr != NULL) {
                if(*ptr != '#') {           // skip comments (first char is a #)
                    
                    key_p = ptr;            // save pointer to key string

                    ptr = strtok(NULL, delim);

                    // second chunk: read value
                    if(ptr != NULL) {

                        value_p = ptr;      // save pointer to value string

                        // check iv key value pair is a valid option
                        if((check_pair(&local_cfg, key_p, value_p)) != RET_SUCCESS) {
                            syslog(LOG_ERR, "Invalid option at line %u\n", line_count);
                            ret_value = RET_ERROR;
                        }
                    }
                    else {
                        syslog(LOG_ERR, "Invalid option at line %u\n", line_count);
                        ret_value = RET_ERROR;
                    }

                }

            }
            line_count++;
        }

        // check error returned by getline
        if((rd_count == -1) && (errno == EINVAL || errno == ENOMEM)) {
            syslog(LOG_ERR, "Error reading configuration file: %s\n", strerror(errno));
            ret_value = RET_ERROR;
        }

        // free buffer allocated by getline
        if(buffer != NULL) {
            free(buffer);
            buffer = NULL;
        }

        // close config file (can't be NULL, checked after fopen())
        fclose(cfg_fp);
        cfg_fp = NULL;
    }

    // if no error occurred, commit configuration changes
    if(ret_value == RET_SUCCESS) {
        *cfg = local_cfg;
    }

    return ret_value;
}

int check_pair(serverConfig *cfg, const char *key_p, const char *value_p) {
    // key value string pair
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];

    // used to convert option values
    unsigned int u_value;

    // return value
    int ret_value = RET_SUCCESS;

    /**************************************************************************/

    strncpy(key, key_p, MAX_KEY_SIZE - 1);
    strncpy(value, value_p, MAX_VALUE_SIZE - 1);

    // check max client opition
    if (strcmp(key, "MAX_CLIENTS") == 0) {
        u_value = (unsigned int) atoi(value);
        if(u_value == 0 || u_value > MAX_CLIENTS) { 
            syslog(LOG_ERR, "Invalid value for max_clients option.\n");
            ret_value = RET_ERROR;
        }
        else {
            cfg->max_clients = u_value;
            syslog(LOG_INFO, "Set option %-12s:%s\n", key, value);
        }
    }   // check listen port opition
    else if(strcmp(key, "LISTEN_PORT") == 0) {
        u_value = (unsigned int) atoi(value);
        if(u_value == 0 || u_value > 65535){
            syslog(LOG_ERR, "Invalid value for listen_port option.\n");
            ret_value = RET_ERROR;
        }
        else {
            cfg->listen_port = (unsigned short) u_value;
            syslog(LOG_INFO, "Set option %-12s:%s\n", key, value);
        }
    }   // check listen addr opition
    else if(strcmp(key, "LISTEN_ADDR") == 0) {  // address value is checked by inet_aton
        strncpy(cfg->listen_addr, value, ADDR_SIZE - 1);
        syslog(LOG_INFO, "Set option %-12s:%s\n", key, value);
    }
    else if(strcmp(key, "LOG_FOLDER") == 0) {
        strncpy(cfg->log_folder, value, MAX_LOG_FOLDER_LEN - 1);
        syslog(LOG_INFO, "Set option %-12s:%s\n", key, value);
    }
    else if(strcmp(key, "LOG_LEVEL") == 0) {
        u_value = (unsigned int) atoi(value);
        if(u_value > LOG_DEBUG) {
            syslog(LOG_ERR, "Invalid value for log_level option.\n");
            ret_value = RET_ERROR;
        }
        else {
            cfg->log_level = (unsigned short) u_value;
            syslog(LOG_INFO, "Set option %-12s:%s\n", key, value);
        }
    }
    else {  // no valid option
        ret_value = RET_ERROR;
    }

    return ret_value;
}