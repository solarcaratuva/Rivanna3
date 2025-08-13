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
        print(f"ERROR: 'pyserial' is not installed. Please install it using 'pip install pyserial'. If on Windows, install pyserial on Windows not WSL.")
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

def colorize(text: str) -> str:
    if len(text) < 20:
        return text
    front = text[5:20]

    if "ERROR" in front or "FAULT" in front:
        return f"\033[91m{text}\033[0m" # red
    elif "WARNING" in front:
        return f"\033[93m{text}\033[0m" # yellow
    elif "DEBUG" in front:
        return f"\033[94m{text}\033[0m" # blue
    else:
        return text

def token_in_line(line: str, tokens: str) -> bool:
    token_list = tokens.lower().split("|")
    line = line.lower()
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
    filteredCount = 0
    errorCount = 0

    try:
        while True:
            # read a line from the serial port
            try:
                text = ser.readline().decode("utf-8").strip()
                messageCount += 1
            except Exception as e:
                text = f"EXCEPTION THROWN: {e}"
                errorCount += 1

            # handle the filter (if there is one)
            if args.filter and not token_in_line(text, args.filter): # if there is a filter and the text does not pass it, skip it
                continue
            if args.filter: # if there is a filter and the text passed it, count it
                filteredCount += 1

            # saving and printing the text
            text_color = colorize(text)
            print(text_color)
            if args.log:
                with open(args.log, "a", encoding="utf-8") as logFile:
                    logFile.write(text + "\n")

    # handle termination of the script
    except KeyboardInterrupt:
        ser.close()
        print(f"Serial connection closed. {messageCount} messages received, {errorCount} exceptions.")
        if args.filter:
            print(f"{filteredCount} messages passed the filter '{args.filter}'")
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