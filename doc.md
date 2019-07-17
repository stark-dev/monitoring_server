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

## System requrirements & Dependencies
### System requirements

The application has been built and tested on `Ubuntu 18.04.1 LTS` running on `x86/64` machine.

Fair CPU and RAM configuration should not affect the execution.

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