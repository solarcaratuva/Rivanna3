import sys
import os
import platform
import subprocess

import re


BOARD_MAP = {
    "driver": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/DriverBoard/DriverBoard.bin",
    "battery": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/BatteryBoard/BatteryBoard.bin",
    "motor": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/Motor/Motor.bin",
    "power": "cmake_build/POWER_BOARD/develop/GCC_ARM/PowerBoard/PowerBoard.bin",
    "wheel": "cmake_build/POWER_BOARD/develop/GCC_ARM/WheelBoard/WheelBoard.bin"}

op_sys = platform.system()

def get_stlink() -> str:
    pattern = r'^(\d+-\d+)'

    cmd = f"usbipd.exe list"
    process = subprocess.run(cmd, shell=True, capture_output=True, check=False)
    if process.returncode != 0: # if error...
        if process.stdout: print(process.stdout.decode())
        if process.stderr: print(process.stderr.decode())
        exit(process.returncode)

    lines = process.stdout.split('\n')
    for line in lines[2:]: # skip first two lines (headers)
        if line == '': # end of list
            break
        if 'ST-Link' in line:
            stlink_id = re.search(pattern,line).group(1)
            return stlink_id

    print("No ST-Link found")
    exit(1)


def attach_stlink(stlink_id: str) -> None:
    # attach (if attach fails, if error is because not bound, then print out)
    cmd = f"usbipd attach --busid {stlink_id} --wsl"
    process = subprocess.run(cmd, shell=True, capture_output=True, check=False)
    if process.returncode != 0: # if error...
        if process.stdout: print(process.stdout.decode())
        if process.stderr: print(process.stderr.decode())
        exit(process.returncode)


def detach_cmd(stlink_id: str) -> None:
    # attach (if attach fails, if error is because not bound, then print out)
    cmd = f"usbipd detach --busid {stlink_id}"
    process = subprocess.run(cmd, shell=True, capture_output=True, check=False)
    if process.returncode != 0: # if error...
        if process.stdout: print(process.stdout.decode())
        if process.stderr: print(process.stderr.decode())
        exit(process.returncode)



def main() -> None:
    if len(sys.argv) != 2:
        print("ERROR: 1 command line argument required: name of the board to upload to")
        sys.exit(1)
    
    board = sys.argv[1].lower().replace("board", "")
    if board not in BOARD_MAP:
        print(f"ERROR: Invalid board name given. Valid board names are: {', '.join(BOARD_MAP.keys())}")
        sys.exit(1)

    if op_sys == "Linux": # WSL, actually Windows
        # create_net_map()

        # cmdlts = BOARD_MAP[board]
        # cmd = f"powershell.exe -Command \"{cmdlts['cmd']}.exe {cmdlts['args']} W:{os.getcwd()}/{cmdlts['path']}\"".replace("/", "\\\\")
        # process = subprocess.run(cmd, shell=True, check=False)

        # delete_net_map()
        # sys.exit(process.returncode)

        # list
        ST_link_num = list_cmd()

        # attach (if attach fails, if error is because not bound, then print out)
        attach_cmd(ST_link_num)

        # st tools BOARD_MAP

        # detach
        detach_cmd(ST_link_num)

    elif op_sys == "Darwin": # Mac
        cmd = f"st-flash --connect-under-reset --reset write {BOARD_MAP[board]}"
        process = subprocess.run(cmd, shell=True, check=False)
        sys.exit(process.returncode)

        # cmdlts = BOARD_MAP[board]
        # cmd = f"{cmdlts['cmd']} {cmdlts['args']} {cmdlts['path']}"
        # process = subprocess.run(cmd, shell=True, check=False)
        # sys.exit(process.returncode)

    elif op_sys == "Windows":
        print("ERROR: This script must be run in WSL")
        sys.exit(1)


if __name__ == "__main__":
    main()
