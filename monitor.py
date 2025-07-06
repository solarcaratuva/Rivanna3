#!/usr/bin/env python3

import argparse
import platform
import subprocess
import shutil
import os
import sys

OS = platform.system()

if OS != "Linux":
    try:
        from serial import Serial
        import serial.tools.list_ports
    except ImportError:
        print(f"ERROR: 'pyserial' is not installed on {OS}. Please install it using 'pip install pyserial' on {OS}.")
        sys.exit(1)


def get_args():
    parser = argparse.ArgumentParser(description="Monitor a board's debug log through a ST-Link")
    parser.add_argument("-l", "--log", type=str, help="Log file path")
    parser.add_argument("-f", "--filter", type=str, help="Filter out messages without this string, use '|' to separate multiple strings")
    return parser.parse_args()

def compress_args(args):
    s = ""
    if args.log:
        args.log = os.path.abspath(args.log)
        process = subprocess.run(f"wslpath -w {args.log}", capture_output=True, shell=True, text=True, check=True)
        path = process.stdout.strip()
        path = path.replace("\\", "\\\\")  # Escape backslashes for Windows command line
        s += f" --log {path}"
    if args.filter:
        s += f" --filter {args.filter}"
    return s

def copy_file_to_windows(wsl_path: str) -> None:
    path = "/" + os.path.join("mnt", "c", "Windows", "Temp", "monitor.py")
    shutil.copy(wsl_path, path)

def token_in_line(line: str, tokens: str) -> bool:
    token_list = tokens.split("|")
    for token in token_list:
        if token in line:
            return True
    return False

def get_correct_port() -> str:
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "stlink" in port.description.lower() or "st-link" in port.description.lower():
            return port.device
    
    print("ERROR: ST-Link could not be found")
    sys.exit(1)


def log(args, port: str) -> None:
    ser = Serial(port, baudrate=921600)
    print(f"Serial connection to {port} established. Now listening...")

    messageCount = 0
    errorCount = 0

    try:
        while True:
            try:
                text = ser.readline().decode("utf-8").strip()
                messageCount += 1
            except Exception as e:
                text = f"EXCEPTION THROWN: {e}"
                errorCount += 1
            if args.filter and not token_in_line(text, args.filter):
                continue
            print(text)
            if args.log:
                with open(args.log, "a", encoding="utf-8") as logFile:
                    logFile.write(text + "\n")
    except KeyboardInterrupt:
        ser.close()
        print(f"Serial connection closed. {messageCount} messages received, {errorCount} exceptions.")
        return
    
def main() -> None:
    args = get_args()

    if OS == "Linux": # WSL, actually Windows
        copy_file_to_windows(os.path.abspath(__file__))
        args_str = compress_args(args)
        subprocess.run(f"cmd.exe /c start \"\" cmd /k python C:\\\\Windows\\\\Temp\\\\monitor.py {args_str}", capture_output=True, shell=True, check=True)

    elif OS == "Darwin" or OS == "Windows":
        port = get_correct_port()
        log(args, port)


if __name__ == "__main__":
    main()