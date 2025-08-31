#!/usr/bin/env python3

import platform
import subprocess
import argparse
import sys
import shutil
import os


BOARD_MAP = {
    "power": "cmake_build/NUCLEO_F413ZH/develop/GCC_ARM/PowerBoard/PowerBoard.bin",
    "telemetry": "cmake_build/POWER_BOARD/develop/GCC_ARM/TelemetryBoard/TelemetryBoard.bin", }

HIL_BOARD_MAP = {}  # board path map specific to the HIL testing system server
CMD_ARGS_ERASE = "-c port=SWD mode=UR -e all"
CMD_ARGS_FLASH = "-c port=SWD mode=UR -rst -d"
OS = platform.system()
EXE_PATH = r"C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"


def get_args():
    parser = argparse.ArgumentParser(
        description="Upload firmware to the board through a ST-Link")
    parser.add_argument("board", type=str,
                        help="Name of the board to upload firmware to")
    parser.add_argument("-s", "--silent", action="store_true",
                        help="Suppress upload output")
    parser.add_argument("--hil", action="store_true",
                        help="Upload firmware to the HIL testing system")
    return parser.parse_args()


def copy_file_to_windows(wsl_path: str) -> None:
    path = "/" + os.path.join("mnt", "c", "Windows", "Temp", "firmware.bin")
    shutil.copy(wsl_path, path)


def is_wsl() -> bool:
    try:
        # Check uname release for "microsoft" (common in WSL1/WSL2)
        if "microsoft" in platform.uname().release.lower():
            return True
        if "WSL_INTEROP" in os.environ:
            return True
        with open("/proc/version", "r") as f:  # Check /proc/version for "Microsoft"
            if "microsoft" in f.read().lower():
                return True
    except Exception:
        pass
    return False


def main() -> None:
    args = get_args()
    board = args.board.lower().replace("board", "")
    if board not in BOARD_MAP:
        print(f"ERROR: '{args.board}' is not a valid board. Valid boards are [{
              ', '.join(BOARD_MAP)}]")
        sys.exit(1)

    match OS:
        case "Linux":
            if not is_wsl():  # non-WSL linux
                path = BOARD_MAP[board]
                cmd_erase = f"STM32_Programmer_CLI {CMD_ARGS_ERASE}"
                cmd_flash = f"STM32_Programmer_CLI {
                    CMD_ARGS_FLASH} {path} 0x08000000"
            else:  # actually WSL in Windows
                copy_file_to_windows(BOARD_MAP[board])
                cmd_erase = f"powershell.exe \"& '{
                    EXE_PATH}' {CMD_ARGS_ERASE}\""
                cmd_flash = f"powershell.exe \"& '{EXE_PATH}' {
                    CMD_ARGS_FLASH} C:\\Windows\\Temp\\firmware.bin 0x08000000\""
        case "Darwin":  # Mac:
            path = BOARD_MAP[board]
            cmd_erase = f"STM32_Programmer_CLI {CMD_ARGS_ERASE}"
            cmd_flash = f"STM32_Programmer_CLI {
                CMD_ARGS_FLASH} {path} 0x08000000"
        case "Windows":
            print("ERROR: run this command in WSL")
            sys.exit(2)
        case _:
            print("ERROR: Unknown OS")
            sys.exit(3)

    process_erase = subprocess.run(
        cmd_erase, capture_output=args.silent, check=False, shell=True)
    if process_erase.returncode != 0:
        sys.exit(process_erase.returncode)
    process_flash = subprocess.run(
        cmd_flash, capture_output=args.silent, check=False, shell=True)
    sys.exit(process_flash.returncode)


if __name__ == "__main__":
    main()
