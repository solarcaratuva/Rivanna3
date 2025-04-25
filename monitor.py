import sys
import argparse
import platform
import subprocess
try:
    from serial import Serial
    import serial.tools.list_ports
except ImportError:
    print("ERROR: \"pyserial\" must be installed. See the Readme for instructions.")
    exit(1)

OS = platform.system()


def get_correct_port() -> str:
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "stlink" in port.description.lower() or "st-link" in port.description.lower():
            return port.device
    
    print("ERROR: ST-Link not found.")
    sys.exit(1)

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

def get_args():
    parser = argparse.ArgumentParser(description="Monitor the ST-Link serial output.")
    # parser.add_argument("-l", "--log", action="store_true", help="Log the output to a file.")
    # parser.add_argument("-p", "--port", type=str, help="Specify the port to use.")
    # parser.add_argument("-s", "--silent", action="store_true", help="Suppress output to the console.")
    parser.add_argument("-p", "--sudo", help="WSL sudo password, required for monitoring on Windows computers")
    parser.add_argument("--wsl-internal", action="store_true", help="DO NOT USE. This is for internal use only.")
    return parser.parse_args()


def log(port: str, log_file_path: str, wsl: bool, stlink_id: str) -> None:
    port = get_correct_port()
    if port is None:
        print("ERROR: ST-Link not found.")
        sys.exit(1)
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
        if wsl:
            detach_stlink(stlink_id)


def main() -> None:
    args = get_args()

    if OS == "Darwin": # Mac
        port = get_correct_port()
        log(port, None, False, None)

    elif OS == "Linux" and not args.wsl_internal:
        stlink_id = get_stlink()
        attach_stlink(stlink_id)

        cmd = f"echo {args.sudo} | sudo -S python3 monitor.py --wsl-internal"
        subprocess.run(cmd, shell=True, check=False)

    elif OS == "Linux" and args.wsl_internal:
        port = get_correct_port()
        log(port, None, True, args.sudo)

if __name__ == "__main__":
    main()
    