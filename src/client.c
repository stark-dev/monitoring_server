#include "../include/client.h"

/*******************************************************************************
* global variables
*******************************************************************************/

volatile sig_atomic_t stop_process;     // global stop flag

/*******************************************************************************
* signal handler
*******************************************************************************/

void sigint_handler(int signal) {
    printf("Caugth signal %d: exiting client...\n", signal);
    stop_process = 1;
}

void sigpipe_handler(int signal) {
    printf("Caugth signal %d: write failed\n", signal);
}

/*******************************************************************************
* main
*******************************************************************************/

int main(int argc, char **argv) {

    /****************************** declarations ******************************/

    // threads
    int n_threads = 1;
    pthread_t *tid_array = NULL;

    void* result;

    // general purpose
    int i;

    
    /**************************************************************************/
    if(argc < 2) {
        printf("Invalid number of arguments.\nUsage: ./client <n>\n\n");
        exit(EXIT_FAILURE);
    }

    // get number of threads to spawn
    n_threads = atoi(argv[1]);

    if(n_threads > MAX_CLIENTS) {       // limit number of clients (only for test purpose)
        n_threads = MAX_CLIENTS;
    }

    // signal handler
    signal(SIGINT, sigint_handler);    // instantiate signal SIGINT handler
    signal(SIGPIPE, sigpipe_handler);  // instantiate signal SIGPIPE handler
    stop_process = 0;                  // set stop flag to zero

    // instantiate thread ids array
    tid_array = malloc(n_threads * sizeof(pthread_t));

    if(tid_array == NULL) { 
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    // release threads
    for(i = 0; i < n_threads; i++) {
        pthread_create(&(tid_array[i]), NULL, sender, (void *)&(ports[i]));
    }

    // join threads
    for(i = 0; i < n_threads; i++) {
        pthread_join(tid_array[i], &result);
    }

    free(tid_array);

    return EXIT_SUCCESS;
}

/*******************************************************************************
* thread sender
*******************************************************************************/

void *sender(void *p) {

    /****************************** declarations ******************************/

    int client_fd;                      // client socket file descriptor (generic)
    struct sockaddr_in socket_info;     // server info

    unsigned short port = *((unsigned short *) p);

    char device_name[MAX_DEV_NAME_LEN]; // device name (dev_<port>)

    uint8_t buffer_tx[MAX_MSG_LEN];     // tx buffer

    int wr_count = 0;                   // write byte count

    int delay = 4;                      // sleep delay (random)

    /**************************************************************************/

    srand(time(NULL));  // random seed

    // init server address info structure
    memset(&socket_info, 0, sizeof(socket_info));
    socket_info.sin_family = AF_INET;
    socket_info.sin_port = htons(DEST_PORT);
    if(inet_aton(DEST_ADDR, &(socket_info.sin_addr)) == 0) {
        printf("Invalid destination IP address");
        return NULL;
    }

    // open socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    printf("PORT: %d - FD: %d\n", port, client_fd);

    // connect to server
    if(connect(client_fd, (struct sockaddr*)&socket_info, sizeof(socket_info)) == -1) {
        perror("connect");
    }
    else {
        printf("Thread %lu connected to %s:%d\n", pthread_self(), DEST_ADDR, DEST_PORT);
        // create device name
        snprintf(device_name, MAX_DEV_NAME_LEN - 1, "dev_%d", port);
        printf("Sending device name %s on port %d\n", device_name, port);

        // evaluate delay between messages
        delay = rand() % 10 + 1; // min 1 sec, max 10 secs
        printf("Set delay to %d seconds on port %d\n", delay, port);
        
        // send device name to server
        wr_count = send(client_fd, device_name, strlen(device_name), 0);

        sleep(delay);

        while(!stop_process && wr_count > 0) {
            // create fake message
            buffer_tx[0] = rand() % 256;
            buffer_tx[1] = rand() % 256;
            buffer_tx[2] = rand() % 256;
            buffer_tx[3] = rand() % 256;

            // write data to server
            wr_count = send(client_fd, buffer_tx, 4, 0);

            printf("Write %d bytes on fd %d\n", wr_count, client_fd);

            sleep(delay);
        }

    }

    // close socket descriptor
    close(client_fd);

    printf("Exiting thread (port %d)...\n", port);

    return NULL;
}