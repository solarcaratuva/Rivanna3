# Rivanna3

Codebase for the embedded code that runs on the Solar Car. Rivanna3 design cycle. <br>
This code was made by and for the Solar Car Team at UVA. Use elsewhere is not recommended. 

## Onboarding Script (optional)

To easily install and configure all required programs, run the [Onboarding Script](TODO). This is optional, but allows you to skip the software installation and configuration mentioned below. 

## Compiling

**Software Installation**

1. Install Git: [Link](https://git-scm.com/download/win)
2. Windows Users: Install Windows Subsystem for Linux 2 (WSL): [Guide](https://learn.microsoft.com/en-us/windows/wsl/install)
    - the WSL command prompt can be activated by entering `wsl` into any command prompt
3. Windows Users: update all packages on WSL by running `sudo apt-get update && sudo apt-get upgrade`
    - you will be prompted for your WSL password, not Windows password
4. Install Docker: [Link](https://docs.docker.com/engine/install/)

**Setting up the compilation / development environment**

1. Make a folder for Solar Car code on your machine, if not done already
    - Windows Users: do this, and the following steps, within WSL using a WSL command prompt
2. Clone this repo into that directory
    - `git clone https://github.com/solarcaratuva/Rivanna3.git`
3. Change directory into the cloned directory 
    - `cd Rivanna3`
4. Run the following command (still in WSL for Windows Users) `docker run --name Rivanna3_compile -it -v $(pwd)/:/root/Rivanna2:Z ghcr.io/solarcaratuva/rivanna2-env`
5. Run `cd Rivanna2` then `mbed-tools deploy`

**Actually Compiling**

1. Attach the command prompt to the container by running `docker attach Rivanna3_compile`
    - the container must be running
2. Run `cd Rivanna2` then `./compile.sh`
    - compilation should take under a minute

Compiled filed are stored in the `cmake_build` directory. <br>
Remember that this compiles the *current* Git branch only. 

**What is actually happening**

1. A Docker container is an isolated space on your computer, only sharing foundational system files with the rest of the computer, as well as limited computational resources. The isolation of the container ensures that everyone has the EXACT same environment when compiling, which prevents phantom, often unsolvable errors from occurring.
2. The `compile.sh` script runs `mbed-tools compile -m UVA_SOLAR_CAR -t GCC_ARM`, which uses MbedOS's own compilation system to compile the code.
3. The compiled code is stored, ready for upload, and reducing the number of files needed to be compiled in the future.

## Uploading

**Software Installation**

Windows Users: install this software on WSL
1. Install [ST-Tools v1.7.0](https://github.com/stlink-org/stlink/releases/tag/v1.7.0)
    - Note that while this package is in package repositories, they are out of date, so you must manually install it 
    - Windows Users: you will need to use `dpkg` to manually install the package in WSL
2. Windows Users: Install [WSL USB](https://github.com/dorssel/usbipd-win)
    - [Guide for common commands](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)

**Alternative, non-recommended Software Installation**
For Windows Users only: Install [ST-Tools](https://github.com/stlink-org/stlink) on windows directly (instead of WSL). Note that the `upload.py` script wasn't made for this, so you will need to run the upload commands directly instead. 

**Actually Uploading**
`cd` into this repo, then run `py upload.py board`
- replace `board` with the name of the board you wish to upload to
- the script does NOT check if you are uploading to the correct board
- you may need to use `python` or `python3` instead of `py`

**What is actually happening**
1. Windows Users: The ST-Link is "attached" to WSL, allowing WSL to access the device
2. the upload script runs the appropriate `st-flash` upload command for the board
    - in reality, the command just writes to memory on the STM32 chip set aside for booting
3. Windows Users: the ST-Link is "detached" from WSL, returning access to Windows


## Serial Monitor and CAN Message Monitor

**Software Installation**

Install PySerial on your computer by running `py -m pip install pyserial`
- you may need to use `python` or `python3` instead of `py`

**Serial Monitor**

Made for basic monitoring, prints all log messages received over serial to the terminal

Attach an ST-Link to a board and your computer, then `cd` into this repo, then run `py monitor.py filepath`
- `filepath` is optional, if included it will save all log messages to the file
- you may need to use `python` or `python3` instead of `py`

**CAN Message Monitoring**

For CAN-based advanced logging, debugging, and analysis, see the [CAN Message Logger](https://github.com/solarcaratuva/CANMessageLogger) project.

## Development

*These are recommendations*
- View and edit code using [VSCode](https://code.visualstudio.com/)
    - install the extensions: `Remote Extensionpack`, `C++ Tools Extensionpack`, `Docker`, `Python`
    - Windows Users: use these extensions to *remote* into where this repo is stored in WSL

## Project Architecture

These notes describe major components of the codebase from a developmental view. **If you are looking for board-specific developmental info, see the `Readme` in that respective board's folder**.

**Folder Structure**: at a high level, the project is structured into folders as follows
- Boards: `WheelBoard`, `ControlBoard`, and `PowerBoard` hold code for those respective boards
- `Common` contains code that is used across the entire project; a significant portion of it is auto-generated
- MbedOS: `mbed-os` and `TARGET_UVA_SOLAR_CAR` contain MbedOS libraries and configuration
- `cmake_build` contains the compiled code from across the project that is actually uploaded to the car
- Legacy: `DriverBoard`, `BatteryBoard`, and `Motor` are legacy boards and code used in *Rivanna2S*, and are only shown in *Rivanna3* for easy reference

**CAN**: under `*board_name*/lib/src` for each board there is a file `*board_name*CANInterface.cpp` that represents the actual CAN connection for each board.
- To send messages: code in `main.cpp` calls the `send()` function, and the message is sent and logged.
- To receive messages: the `message_handler()` function waits for CAN messages. When one is received, the function logs the message, and checks if the message is one of the types of CAN messages it is waiting for. If so, the function calls the appropriate `handle()` function in `main.cpp`; if not, the message is disregarded.
