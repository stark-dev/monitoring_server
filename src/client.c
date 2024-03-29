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
    thread_params *tparams = NULL;

    void* result;

    // connection
    char address[16] = DEST_ADDR;
    unsigned short port = DEST_PORT;

    // general purpose
    int i;

    
    /**************************************************************************/
    if(argc < 4) {
        printf("Invalid number of arguments.\nUsage: ./client <n> <addr> <port>\n\n");
        exit(EXIT_FAILURE);
    }

    // get number of threads to spawn
    n_threads = atoi(argv[1]);
    strncpy(address, argv[2], 15);
    port = atoi(argv[3]);

    if(n_threads > MAX_CLIENTS) {       // limit number of clients (only for test purpose)
        n_threads = MAX_CLIENTS;
    }

    // signal handler
    signal(SIGINT, sigint_handler);    // instantiate signal SIGINT handler
    signal(SIGPIPE, sigpipe_handler);  // instantiate signal SIGPIPE handler
    stop_process = 0;                  // set stop flag to zero

    // instantiate thread ids array
    tid_array = malloc(n_threads * sizeof(pthread_t));
    tparams = malloc(n_threads * sizeof(thread_params));

    if(tid_array == NULL) { 
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    // release threads
    for(i = 0; i < n_threads; i++) {
        strncpy(tparams[i].dest_addr, address, 15);
        tparams[i].dest_port = (unsigned short) port;
        tparams[i].thread_id = (unsigned int) i;

        pthread_create(&(tid_array[i]), NULL, sender, (void *)&(tparams[i]));
    }

    // join threads
    for(i = 0; i < n_threads; i++) {
        pthread_join(tid_array[i], &result);
    }

    free(tid_array);
    free(tparams);

    return EXIT_SUCCESS;
}

/*******************************************************************************
* thread sender
*******************************************************************************/

void *sender(void *p) {

    /****************************** declarations ******************************/

    int client_fd;                      // client socket file descriptor (generic)
    struct sockaddr_in socket_info;     // server info

    thread_params tp = *((thread_params *) p); 

    char device_name[MAX_DEV_NAME_LEN]; // device name (dev_<id>)

    uint8_t buffer_tx[MAX_MSG_LEN];     // tx buffer

    int wr_count = 0;                   // write byte count

    int delay = 4;                      // sleep delay (random)

    uint32_t data;                      // random data

    /**************************************************************************/

    srand(time(NULL));  // random seed

    printf("Thread %lu: dest addr %s - dest port %d - device id %d\n", pthread_self(), tp.dest_addr, tp.dest_port, tp.thread_id);

    // init server address info structure
    memset(&socket_info, 0, sizeof(socket_info));
    socket_info.sin_family = AF_INET;
    socket_info.sin_port = htons(tp.dest_port);
    if(inet_aton(tp.dest_addr, &(socket_info.sin_addr)) == 0) {
        printf("Invalid destination IP address\n");
        return NULL;
    }

    // open socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    // connect to server
    if(connect(client_fd, (struct sockaddr*)&socket_info, sizeof(socket_info)) == -1) {
        perror("connect");
    }
    else {
        printf("Thread %lu: connected to %s:%d\n", pthread_self(), tp.dest_addr, tp.dest_port);
        // create device name
        snprintf(device_name, MAX_DEV_NAME_LEN - 1, "dev_%d", tp.thread_id);
        printf("Thread %lu: sending device name %s\n", pthread_self(), device_name);

        // evaluate delay between messages
        delay = rand() % 10 + 1; // min 1 sec, max 10 secs
        printf("Thread %lu: set delay to %d seconds on device %d\n", pthread_self(), delay, tp.thread_id);
        
        // send device name to server
        wr_count = send(client_fd, device_name, strlen(device_name), 0);

        while(!stop_process && wr_count > 0) {
            sleep(delay);
            
            // create fake measure
            data = rand() % 10;

            // write 4 bytes in big endian format on tx buffer
            if((write_32(&data, (void *) buffer_tx, BE)) != sizeof(uint32_t)) {
                printf("Thread %lu: error while writing data\n", pthread_self());
            }
            else {
                // write data to server
                wr_count = send(client_fd, buffer_tx, 4, 0);
                printf("Thread %lu: write %d bytes on device %d (fd %d)\n", pthread_self(), wr_count, tp.thread_id, client_fd);
            }
        }

    }

    // close socket descriptor
    close(client_fd);

    printf("Thread %lu: exit\n", pthread_self());

    pthread_exit(NULL);
}