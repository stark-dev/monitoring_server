# `monitoring_server`

## TL;DR
```
git clone git@github.com:stark-dev/monitoring_server.git
cd monitoring_server
mkdir build && cd build
cmake ..
make

./monitoring_server
```

## Description
`monitoring_server` is a monitoring application written in C. Its main task is to read and log data received from a set of external devices through a TCP socket. It is assumed that each device has network access and that it is able to reach the host machine that runs the application. The target host for `monitoring_server` is a `x86/64` machine.

### Implemented features
- read and print the number of messages received from each connected device (total message count is printed when the device disconnets)
- log received data from each device to a log file (<dev_name>.log)
- `syslog` integration with configurable log level (POSIX compliant)
- easy run time configuration using a text file

On connection, each device sends a first message with a unique name (a string of maximum 99 characters terminated by NULL), which is used to create a dedicated log file. Each subsequent message represents a measure, encoded on a 4 bytes unsigned integer (`uint32_t`). If the maximum number of connected clients is reached, the server application rejects new incoming connections by closing the corresponding file descriptor.

### Future improvements
- multi-thread implementation to improve scalability and handle a greater number of clients
- encrypted TLS connection and client authentication using PSK
- log measures to a database (i.e., `SQLite`)

## System requrirements & Dependencies
### System requirements

The application has been built and tested on `Ubuntu 18.04.1 LTS` running on a `x86/64` machine.

CPU usage and memory footprint are typically very low.

### Dependencies
- `build-essentials`
    - `gcc >= 4:7.2`
    - `libc6-dev`
    - `make >= 4.1`
- `git >= 2.17.1`
- `cmake >= 3.10.2`

## Build steps

1. First, clone the repository from GitHub and switch to `master` branch (be sure it is up to date)
```
git clone git@github.com:stark-dev/monitoring_server.git
cd monitoring_server
git checkout master
git pull
```

2. Create a build directory and build the project
```
mkdir build && cd build
cmake ..
make
```
No install target is required to test the application locally.

After compiling, two different executables are produced: `monitoring_server`,  which is the application in charge of monitoring multiple devices, and `client`, which emulates `n` devices by means of separate threads.

To start the server, just run the executable file:
```
./monitoring_server
```

## Testing
`monitoring_server` may be tested either with the included `client` application or by using `telnet`.

### `client` application
The client application emulates `n` devices that concurrently send messages to the server. The interval between two subsequent messages is a random integer (from 1 to 10 seconds). To start the client type
```
./client <n> <address> <port>
```
- `n` is the number of devices to emulate (each one spawns a new thread)
- `address` is the server IPv4 address
- `port` is the server listening port

For instance,
```
./client 5 127.0.0.1 1234
```
will run 5 threads, each of them sending messages to`localhost` on port `1234`.
The maximum number of devices that can be emulated is 30. Each thread stops its execution if the socket is closed by the server.

### `telnet`
It is sufficient to type
```
telnet localhost 1234
```
to connect to the server. Type the device name as the first message, then try to send some characters. They will be converted to a 4 bytes integer value, thus it may be difficult to send valid data using `telnet`. It is intended only to test the connection and message count. Close the connection on client side and check the total count of received messages.

## Log messages

By default log messages are printed on `stderr`, as well as on the standard system log using `syslog`.
```
journalctl -fxet monitoring_server
```

## Configuration
No command line arguments are required. The following options can be configured by modifying the configuration file `config.txt` available in the root folder of the repository:
- `MAX_CLIENTS` : maximum number of client connections accepted (default value is 10)
- `LISTEN_PORT` : listening port opened by server (default `1234`)
- `LISTEN_ADDR` : socket bind address (default `0.0.0.0`)
- `LOG_FOLDER`  : folder that stores log files, one per device (default `./log`)
- `LOG_LEVEL` : shows log messages up to specified level (from `0` to `7`, default `7`)

After saving changes, restart the server application.