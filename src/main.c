#include "../include/main.h"

/*******************************************************************************
* global variables
*******************************************************************************/

volatile sig_atomic_t stop_process;     // global stop flag

/*******************************************************************************
* signal handler
*******************************************************************************/

void sigint_handler(int signal) {
    syslog(LOG_INFO, "Exiting monitoring server...\n");
    stop_process = 1;
}

/*******************************************************************************
* main
*******************************************************************************/

int main(int argc, char **argv) {

    /****************************** declarations ******************************/

    // networking
    int server_fd;      // server socket file descriptor
    int client_fd;      // client socket file descriptor (generic)

    struct sockaddr_in server_socket;   // server info
    struct sockaddr_in client_socket;   // client info
    unsigned int client_socket_len = sizeof(client_socket);

    // general purpose
    unsigned int i;
    uint8_t found;

    // messages
    uint8_t rd_buffer[MAX_MSG_LEN];     // read buffer
    uint32_t data;                      // temporary data
    unsigned int len = -1;

    // select and file descriptors
    fd_set read_fds;                    // file descriptor reading set (for select)
    int fd_ready = -1;                  // select result
    unsigned int *client_fd_array;      // client file descriptor array
    int max_fd = -1;                    // max file descriptor value (needed by select)
    int optval = 1;                     // socket option config value (enable configuration)
    struct timeval select_to;           // select timeout structure

    // device name and metrics
    char **device_name;                 // store device names
    char *token = NULL;
    const char *delim = " \r\n\0";      // split on CR, LN

    unsigned long *message_count;       // client messages count
    uint8_t *device_init;               // initialisation flag for each device
    FILE **log_file_ptr;                // file pointer array
    char log_file_name[MAX_LOG_FILE_LEN];

    // configuration
    serverConfig config;

    /**************************************************************************/

    // start system logger and register as 'monitoring_server'
    open_syslog("monitoring_server");

    // init configuration
    if ((read_config(&config)) != 0) {
        syslog(LOG_WARNING, "Invalid configuration, using default\n");
    }

    set_log_level(config.log_level);

    // create log folder
    if((create_log_folder(config.log_folder)) == -1) {
        exit(EXIT_FAILURE);
    }

    // init dynamic structures which depend on configuration value
    client_fd_array = (unsigned int*) malloc(config.max_clients * sizeof(unsigned int));
    message_count   = (unsigned long *) malloc(config.max_clients * sizeof(unsigned long));
    device_init     = (uint8_t *) malloc(config.max_clients * sizeof(uint8_t));
    log_file_ptr    = (FILE **) malloc(config.max_clients * sizeof(FILE *));

    device_name = (char **) malloc(config.max_clients * sizeof(char *));
    for(i = 0; i < config.max_clients; i++) {
        device_name[i] = (char *) calloc(MAX_DEV_NAME_LEN, sizeof(char));   // initialize to zeros
    }

    // signal handler
    signal(SIGINT, sigint_handler);    // instantiate signal handler
    stop_process = 0;                   // set stop flag to zero

    // init server address info structure
    memset(&server_socket, 0, sizeof(server_socket));
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(config.listen_port);
    if(inet_aton(config.listen_addr, &(server_socket.sin_addr)) == 0) {
        syslog(LOG_ERR, "Invalid listen IP address: %s\n", config.listen_addr);
        exit(EXIT_FAILURE);
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);    // domain: IPv4, type: stream, protocol: default (TCP)
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));      // allow immediate re-binding of port
    
    for(i = 0; i < config.max_clients; i++) {
        client_fd_array[i] = 0;     // reset client socket file descriptors
        message_count[i] = 0;       // reset message count
        device_init[i] = 0;         // reset device init flag
        log_file_ptr[i] = NULL;     // init log file pointers
    }

    // socket binding and listening
    if(bind(server_fd, (struct sockaddr*)&(server_socket), sizeof(server_socket)) == -1) {
        syslog(LOG_ERR, "Socket bind failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, MAX_INCOMING) != 0) {
        syslog(LOG_ERR, "Socket listen failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Open listening socket on address %s:%d (fd: %d)\n", inet_ntoa(server_socket.sin_addr), ntohs(server_socket.sin_port), server_fd);

    // main loop
    while(!stop_process) {

        // define select timeout
        select_to.tv_sec = SELECT_TIMEOUT_SEC;
        select_to.tv_usec = 0;

        FD_ZERO(&read_fds);                 // reset read file descriptors
        FD_SET(server_fd, &read_fds);       // add server file descriptor

        max_fd = server_fd;                 // init max fd to server fd value

        // check active client fds
        for(i = 0; i < config.max_clients; i++) {
            client_fd = client_fd_array[i];
            if(client_fd > 0) {
                FD_SET(client_fd, &read_fds);
                if(client_fd > max_fd) {
                    max_fd = client_fd;     // update max fd value
                }
            }
        }

        fd_ready = select(max_fd + 1, &read_fds, NULL, NULL, &select_to);   // 0 -> no read fd active, >0 -> some fds ready, -1 -> error

        if(fd_ready < 0) {
            if (errno != EINTR) {
                syslog(LOG_ERR, "Select failed: %s\n", strerror(errno));
            }
        }
        else if(fd_ready == 0) {
            syslog(LOG_INFO, "No incoming connections: select timeout reached\n");
        }
        else {
            if(FD_ISSET(server_fd, &read_fds)) {                             // server socket fd set -> incoming connection
                // accept and assign new fd to client
                if((client_fd = accept(server_fd, (struct sockaddr*)&client_socket, &client_socket_len)) < 0) {
                    syslog(LOG_ERR, "Socket accept failed: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                syslog(LOG_INFO, "New connection from %s:%d (fd %d)\n", inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port),  client_fd);

                // add client file descriptor to first available slot in client_fd_array
                found = 0;
                i = 0;
                while(!found && i < config.max_clients) {
                    if(client_fd_array[i] == 0) {
                        client_fd_array[i] = client_fd;
                        found = 1;
                    }
                    i++;
                }

                // reached max connections limit, closing new connection
                if(!found) {
                    syslog(LOG_WARNING, "Reached client limit\n");
                    close(client_fd);
                }
            }

            // scan all client file descriptors to find which one is active
            for(i = 0; i < config.max_clients; i++) { 
                client_fd = client_fd_array[i];

                // clear read buffer
                memset((void* ) rd_buffer, 0, MAX_MSG_LEN);

                if(FD_ISSET(client_fd, &read_fds)) {
                    len = (unsigned int) read(client_fd, rd_buffer, MAX_MSG_LEN - 1); // read data

                    if(len > 0) {                                           // new data available
                        // first message from device, set name
                        if(!device_init[i]) {
                            token = strtok((char *) rd_buffer, delim);      // remove trailing CR/LF
                            syslog(LOG_INFO, "Device on fd %d registered as %s\n", client_fd, token);

                            strncpy(device_name[i], token, MAX_DEV_NAME_LEN - 1);
                            device_init[i] = 1;
                            message_count[i] = 0;                           // reset message count for current client

                            // create log file for connected device
                            snprintf(log_file_name, MAX_LOG_FILE_LEN - 1, "%s/%s.log", config.log_folder, device_name[i]);
                            log_file_ptr[i] = fopen(log_file_name, "a");
                            if(log_file_ptr[i] == NULL) {
                                syslog(LOG_ERR, "Unable to open log file %s: %s\n", log_file_name, strerror(errno));
                                exit(EXIT_FAILURE);
                            }
                        }
                        else {
                            syslog(LOG_DEBUG, "New message of size %d from %s (fd = %d)\n", len, device_name[i], client_fd);
                            // read 4 bytes data in big endian format from read buffer
                            if((read_32((void *) rd_buffer, &data, BE)) != sizeof(data)) {
                                syslog(LOG_WARNING, "Invalid data length\n");
                            }
                            else {
                                // write to device log file
                                fprintf(log_file_ptr[i], "timestamp: %lu - value: %u\n", (unsigned long) time(NULL), data);
                            }
                            message_count[i]++;                             // increment message count
                        }
                    }
                    else {                                                  // client closed connection
                        close(client_fd);

                        syslog(LOG_INFO, "Closing socket on fd: %d\n", client_fd);
                        syslog(LOG_INFO, "Received %lu messages from %s\n", message_count[i], device_name[i]);

                        device_init[i] = 0;                                     // reset device init flag
                        client_fd_array[i] = 0;                                 // reset file descriptor value
                        memset((void *) (device_name[i]), 0, MAX_DEV_NAME_LEN); // clear file name
                        if(log_file_ptr[i] != NULL) {
                            fclose(log_file_ptr[i]);                            // close log file and reset file pointer
                            log_file_ptr[i] = NULL;
                        }
                    }
                }
            }
        }
    }

    // close open client sockets
    for(i = 0; i < config.max_clients; i++) {
        if (client_fd_array[i] > 0) {
            client_fd = client_fd_array[i];
            syslog(LOG_INFO, "Closing socket on fd: %d\n", client_fd);
            syslog(LOG_INFO, "Received %lu messages from %s\n", message_count[i], device_name[i]);
            close(client_fd);
            device_init[i] = 0;
            client_fd_array[i] = 0;
            memset((void *) (device_name[i]), 0, MAX_DEV_NAME_LEN);
            if(log_file_ptr[i] != NULL) {
                fclose(log_file_ptr[i]);    // close log file and reset file pointer
                log_file_ptr[i] = NULL;
            }
        }
    }

    close(server_fd);   // close server fd

    // free up memory
    free(client_fd_array);
    free(message_count);

    for(i = 0; i < config.max_clients; i++) {
        free(device_name[i]);
    }
    free(device_name);
    free(device_init);
    free(log_file_ptr);

    close_syslog();

    return EXIT_SUCCESS;
}

/*******************************************************************************
* create log folder
*******************************************************************************/

int create_log_folder(const char *name) {

    char folder_name[MAX_LOG_FOLDER_LEN];
    struct stat st = {0};
    int ret = 0;

    strncpy(folder_name, name, MAX_LOG_FOLDER_LEN - 1);

    // create log folder if does not exits
    if(stat(folder_name, &st) == -1) {
        syslog(LOG_INFO, "Creating log folder '%s'\n", folder_name);
        ret = mkdir(folder_name, S_IRWXU | S_IRWXG | S_IROTH);
    }
    else {
        syslog(LOG_INFO, "Log folder '%s' exists\n", folder_name);
    }

    if(ret == -1) {
        syslog(LOG_ERR, "Log folder creation failed: %s\n", strerror(errno));
    }

    return ret;
}

/*******************************************************************************
* log utils
*******************************************************************************/
void set_log_level(int level) {
    syslog(LOG_INFO, "Set maximum log level to %d\n", level);
    setlogmask(LOG_UPTO (level));
    return;
}

void open_syslog(const char *name) {
    #if CONSOLE_LOG == 1
        openlog(name, LOG_PERROR, 0);
    #else
        openlog(name, 0, 0);
    #endif
    
    syslog(LOG_INFO, "Starting system log\n");
    return;
}

void close_syslog() {
    syslog(LOG_INFO, "Closing system log\n");
    closelog();
    return;
}
