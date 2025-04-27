import sys
import argparse
import platform
import subprocess
import time
import re
try:
    from serial import Serial
    import serial.tools.list_ports
except ImportError:
    print("ERROR: \"pyserial\" must be installed. See the Readme for instructions.")
    exit(1)


OS = platform.system()


def get_args():
    parser = argparse.ArgumentParser(description="Monitor a board's debug log through a ST-Link")
    parser.add_argument("-s", "--silent", action="store_true", help="Suppress monitor output")
    parser.add_argument("-p", "--sudo", help="WSL sudo password, required for monitoring on Windows computers")
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


def get_correct_port() -> str:
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "stlink" in port.description.lower() or "st-link" in port.description.lower():
            return port.device
    
    print("ERROR: ST-Link not found.")
    sys.exit(1)


def log():
    log_file_path = None

    port = get_correct_port()        
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
            print(text)
            if log_file_path:
                with open(log_file_path, "a") as log:
                    log.write(text + "\n")
    except KeyboardInterrupt:
        ser.close()
        print(f"Serial connection closed. {messageCount} messages received, {errorCount} exceptions.")
        return
    
def main() -> None:
    args = get_args()

    if OS == "Linux": # WSL, actually Windows
        sudo = args.sudo if args.sudo else None
        if not sudo:
            print("ERROR: Sudo password required for monitoring on Windows computers")
            exit(1)

        stlink_id = get_stlink()
        attach_stlink(stlink_id)
        time.sleep(1)
        process = subprocess.run(f"echo {sudo} | sudo -S chmod 666 /dev/ttyACM0", shell=True, capture_output=False, check=False) # give access to the serial port to WSL user accounts 
        if process.returncode != 0: exit(1)

        log()

        detach_stlink(stlink_id)

    elif OS == "Darwin": # Mac
        log()
    
    elif OS == "Windows":
        print("ERROR: This script must be run in WSL")


if __name__ == "__main__":
    main()