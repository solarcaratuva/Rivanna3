import platform
import subprocess
import argparse
import re


BOARD_MAP = {
    "driver": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/DriverBoard/DriverBoard.bin",
    "battery": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/BatteryBoard/BatteryBoard.bin",
    "motor": "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/Motor/Motor.bin",
    "power": "cmake_build/POWER_BOARD/develop/GCC_ARM/PowerBoard/PowerBoard.bin",
    "wheel": "cmake_build/POWER_BOARD/develop/GCC_ARM/WheelBoard/WheelBoard.bin",
    "telemetry": "cmake_build/POWER_BOARD/develop/GCC_ARM/TelemetryBoard/TelemetryBoard.bin",}

HIL_BOARD_MAP = {} # board path map specific to the HIL testing system server

OS = platform.system()


def get_args():
    parser = argparse.ArgumentParser(description="Upload firmware to the board through a ST-Link")
    parser.add_argument("board", type=str, help="Name of the board to upload firmware to")
    parser.add_argument("-s", "--silent", action="store_true", help="Suppress upload output")
    parser.add_argument("--hil", action="store_true", help="Upload firmware to the HIL testing system")
    return parser.parse_args()

def get_stlink() -> str:
    """Get the busid of the ST-Link device"""

    cmd = "usbipd.exe list" # .exe runs the program in Windows from WSL
    process = subprocess.run(cmd, shell=True, capture_output=True, check=False)
    if process.returncode != 0: # if error...
        if process.stdout: print(process.stdout.decode())
        if process.stderr: print(process.stderr.decode())
        exit(process.returncode)

    lines = process.stdout.decode().split('\n')
    for line in lines[2:]: # skip first two lines (headers)
        if 'Persisted:' in line: # end of list
            break
        if 'ST-Link' in line:
            stlink_id = re.search(r'^(\d+-\d+)', line).group(1)
            return stlink_id

    print("No ST-Link found")
    exit(1)


def attach_stlink(stlink_id: str) -> None:
    """Attach the ST-Link device to WSL"""

    cmd = f"usbipd.exe attach --busid {stlink_id} --wsl"
    process = subprocess.run(cmd, shell=True, capture_output=True, check=False)
    if process.returncode != 0 and "usbipd: error: Device is not shared;" in process.stderr.decode(): # if not bound error
        print("Run the following command in a WINDOWS command prompt with ADMIN privileges to make the ST-Link available. Then rerun this script:")
        print(f"usbipd bind --busid {stlink_id}")
        exit(1)
    elif process.returncode != 0: # if other error
        if process.stdout: print(process.stdout.decode())
        if process.stderr: print(process.stderr.decode())
        exit(process.returncode)


def detach_stlink(stlink_id: str) -> None:
    """Detach the ST-Link device from WSL"""

    cmd = f"usbipd.exe detach --busid {stlink_id}"
    process = subprocess.run(cmd, shell=True, capture_output=True, check=False)
    if process.returncode != 0: # if error...
        if process.stdout: print(process.stdout.decode())
        if process.stderr: print(process.stderr.decode())
        exit(process.returncode)


def upload(path: str, silent: bool) -> int:
    """Run the ST-Tools upload command to upload the firmware to the board"""

    cmd = f"st-flash --connect-under-reset --reset write {path} 0x8000000"
    process = subprocess.run(cmd, shell=True, check=False, capture_output=silent)
    if silent and process.returncode != 0:
        print("Upload failed")
    return process.returncode


def main() -> None:
    args = get_args()
    board = args.board.lower().replace("board", "")
    if board not in BOARD_MAP:
        print(f"ERROR: Invalid board name given. Valid board names are: {', '.join(BOARD_MAP.keys())}")
        exit(1)

    if OS == "Linux" and not args.hil: # WSL, actually Windows
        stlink_id = get_stlink()
        attach_stlink(stlink_id)

        exit_code = upload(BOARD_MAP[board], args.silent)

        detach_stlink(stlink_id)
        exit(exit_code)

    elif OS == "Linux" and args.hil: # HIL testing system server
        exit_code = upload(HIL_BOARD_MAP[board], False)
        exit(exit_code)

    elif OS == "Darwin": # Mac
        exit_code = upload(BOARD_MAP[board], args.silent)
        exit(exit_code)

    elif OS == "Windows":
        print("ERROR: This script must be run in WSL")
        exit(1)

    else:
        print("ERROR: Unknown OS")
        exit(1)


if __name__ == "__main__":
    main()
