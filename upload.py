import sys
import platform
import subprocess
import time

BOARD_MAP = {
    "driver": "st-flash --connect-under-reset --reset write cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/DriverBoard/DriverBoard.bin 0x8000000",
    "battery": "st-flash --connect-under-reset --reset write cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/BatteryBoard/BatteryBoard.bin 0x8000000",
    "motor": "st-flash --connect-under-reset --reset write cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/Motor/Motor.bin 0x8000000",
    "test": "echo 'This is a connectivity test, not a real board'; lsusb"}
os = platform.system()

def windows_stlink_attach() -> str:
    usb_id = None
    process = subprocess.run("usbipd list", shell=True, capture_output=True)
    output = process.stdout.decode("utf-8").split("Persisted:")[0]
    for line in output.split("\n"):
        if "ST-Link" in line:
            usb_id = line.split(" ")[0]
            break
    if usb_id is None:
        print("ERROR: No ST-Link found")
        exit(1)

    process = subprocess.run(f"usbipd bind --busid {usb_id}", shell=True, capture_output=True)
    if process.returncode != 0:
        print(f"ERROR: Failed to bind the ST-Link. Command output: \n{process.stdout.decode('utf-8')}\n{process.stderr.decode('utf-8')}")
        exit(1)
    process = subprocess.run(f"usbipd attach --wsl --busid {usb_id}", shell=True, capture_output=True)
    if process.returncode != 0:
        print(f"ERROR: Failed to attach the ST-Link to WSL. Command output: \n{process.stdout.decode('utf-8')}\n{process.stderr.decode('utf-8')}")
        exit(1)
    return usb_id


def windows_stlink_detach(usb_id: str) -> None:
    process = subprocess.run(f"usbipd detach --busid {usb_id}", shell=True, capture_output=True)
    if process.returncode != 0:
        print(f"ERROR: Failed to detach the ST-Link from WSL. Command output: \n{process.stdout.decode('utf-8')}\n{process.stderr.decode('utf-8')}")
        exit(1)


def main() -> None:
    if len(sys.argv) != 2:
        print("1 command line argument required: name of the board to upload to")
        sys.exit(1)
    board = sys.argv[1].lower().replace("board", "")
    if board not in BOARD_MAP:
        print(f"ERROR: Invalid board name given. Valid board names are: {', '.join(BOARD_MAP.keys())}")
        sys.exit(1)

    if os == "Windows":
        usb_id = windows_stlink_attach()
        time.sleep(1) # new USB connections need some time to be recognized

    cmd = BOARD_MAP[board]
    if os == "Windows":
        cmd = f"wsl sh -c \"{cmd}\""
    process = subprocess.run(cmd, shell=True)

    if os == "Windows":
        windows_stlink_detach(usb_id)
    elif os == "Linux" and process.returncode != 0:
        print("\nWARNING: It appears this program is being run from inside WSL. You must manually attach the ST-Link to WSL, or run this program from Windows instead")
    exit(process.returncode)


if __name__ == "__main__":
    main()
