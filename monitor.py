from serial import Serial
import serial.tools.list_ports
import sys


def get_correct_port() -> str:
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "stlink" in port.description.lower() or "st-link" in port.description.lower():
            return port.device
    return None


# MAIN
log_file_path = None
if len(sys.argv) >= 2:
    log_file_path = sys.argv[1]

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
    