# Documentation: `monitoring_server`

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
`monitoring_server` is a TCP based monitoring application that reads and logs data sent by a set of external devices.
### Implemented features
- read and print the number of messages received by each connected device
- log received data from each device to a separate log file
- `syslog` integration with log level handling (POSIX compliant)
- easy run time configuration using a text file

On connection, each device sends a first message with a unique name, which is used to create a dedicated log file. Each following message is measure encoded on a 4 bytes unsigned integer (`uint32_t`). 

### Future improvements
- multi-thread implementation to improve scalability and handle a greater number of clients
- encrypted TLS connection and client authentication using PSK
- log measures to database (i.e., `SQLite`)

## Implementation

## Test
`monitoring_server` may be tested either with the included `client` (see below) or by using `telnet`. It is sufficient to type
```
telnet localhost 1234
```
to connect to the server. The first message sent will be saved as the device name. Subsequent messages will be read as 4 bytes integer measures(it may be difficult to send valid data using `telnet`, it is intended only to test connection and message count).

If the maximum number of connected clients is reached, the server application rejects new incoming connections by closing the corresponding file descriptor.

Log messages can be either read on the console output or by opening the system log utility:
```
journalctl -fxet server
```

The application has been tested using the `valgrind` utility to ensure no memory leak is present.

## System requrirements & Dependencies
### System requirements

The application has been built and tested on `Ubuntu 18.04.1 LTS` running on a `x86/64` machine.

CPU and RAM configuration should not affect the execution.

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

2. Create build directory and build the project
```
mkdir build && cd build
cmake ..
make
```
No install target is required to test the application locally.

After compiling, two different executables are produced: `monitoring_server`,  which is the application in charge of monitoring multiple devices, and `client`, which emulates a set of `n` devices by means of separate threads.

## `monitoring_server`
To start the server, just run the executable file:
```
./monitoring_server
```
No command line arguments are required. The following options can be configured by modifying the configuration file `config.txt` available in the root folder of the repository:
- `MAX_CLIENTS` : maximum number of client connections accepted (default value is 10)
- `LISTEN_PORT` : listening port opened by server (default `1234`)
- `LISTEN_ADDR` : socket bind address (default `0.0.0.0`)
- `LOG_FOLDER`  : folder that stores log files, one per device (default `./log`)
- `LOG_LEVEL` : shows log messages up to specified level (from `0` to `7`, default `7`)

## `client`
The client application emulates `n` devices (one thread per device) that concurrently send messages to the server. To start the client type
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
will run 5 threads, each of them sending messages to the `localhost` address on port `1234`.

The maximum number of devices that can be emulated is 20.