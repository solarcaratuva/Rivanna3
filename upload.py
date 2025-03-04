import sys
import os
import platform
import subprocess

BOARD_MAP = {
    "driver": {"cmd": "st-flash", "args": "--connect-under-reset --reset write", "path": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/DriverBoard/DriverBoard.bin 0x8000000"},
    "battery": {"cmd": "st-flash",  "args":"--connect-under-reset --reset write", "path": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/BatteryBoard/BatteryBoard.bin 0x8000000"},
    "motor": {"cmd": "st-flash", "args": "--connect-under-reset --reset write", "path": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/Motor/Motor.bin 0x8000000"},
    "power": {"cmd": "st-flash", "args": "--connect-under-reset --reset write", "path": "cmake_build/POWER_BOARD/develop/GCC_ARM/PowerBoard/PowerBoard.bin 0x8000000"},
    "wheel": {"cmd": "st-flash", "args": "--connect-under-reset --reset write", "path": "cmake_build/POWER_BOARD/develop/GCC_ARM/WheelBoard/WheelBoard.bin 0x8000000"}}
op_sys = platform.system()

def create_net_map() -> None:
    """
    Runs the `net` command in Windows (yes even in WSL) to mount the WSL filesystem path as a network drive in Windows, named `W:`
    """
    cmd = f"powershell.exe -Command \"net.exe use W: \\\\\\\\wsl$\\\\{os.getenv('WSL_DISTRO_NAME')}\""
    process = subprocess.run(cmd, shell=True, check=False, capture_output=True)
    if process.returncode != 0:
        if process.stdout:
            print(process.stdout.decode())
        if process.stderr:
            print(process.stderr.decode())
        sys.exit(1)

def delete_net_map() -> None:
    """
    Deletes the network drive mount `W:`
    """
    cmd = "powershell.exe -Command \"net.exe use W: /delete \""
    process = subprocess.run(cmd, shell=True, check=False, capture_output=True)
    if process.returncode != 0:
        if process.stdout:
            print(process.stdout.decode())
        if process.stderr:
            print(process.stderr.decode())

def main() -> None:
    if len(sys.argv) != 2:
        print("ERROR: 1 command line argument required: name of the board to upload to")
        sys.exit(1)
    
    board = sys.argv[1].lower().replace("board", "")
    if board not in BOARD_MAP:
        print(f"ERROR: Invalid board name given. Valid board names are: {', '.join(BOARD_MAP.keys())}")
        sys.exit(1)

    if op_sys == "Linux": # WSL, actually Windows
        create_net_map()

        cmdlts = BOARD_MAP[board]
        cmd = f"powershell.exe -Command \"{cmdlts['cmd']}.exe {cmdlts['args']} W:{os.getcwd()}/{cmdlts['path']}\"".replace("/", "\\\\")
        process = subprocess.run(cmd, shell=True, check=False)

        delete_net_map()
        sys.exit(process.returncode)

    elif op_sys == "Darwin": # Mac
        cmdlts = BOARD_MAP[board]
        cmd = f"{cmdlts['cmd']} {cmdlts['args']} {cmdlts['path']}"
        process = subprocess.run(cmd, shell=True, check=False)
        sys.exit(process.returncode)

    elif op_sys == "Windows":
        print("ERROR: This script must be run in WSL")
        sys.exit(1)


if __name__ == "__main__":
    main()
