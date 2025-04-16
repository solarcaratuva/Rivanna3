import subprocess
import argparse


CONTAINER = "Rivanna3_compile"

arg_parser = argparse.ArgumentParser(description="Compile Rivanna3 code in the Docker container.")
arg_parser.add_argument("args", nargs="*", help="Arguments to pass to `compile.sh`.")
arg_parser.add_argument("-c", "--clean", action="store_true", help="Clean the build before compiling.")
arg_parser.add_argument("-s", "--silent", action="store_true", help="Suppress output from the compilation process.")
args = arg_parser.parse_args()

process = subprocess.run("docker --help", shell=True, capture_output=True, text=True)
if process.returncode != 0:
    print("Docker is not running. Please start Docker and try again.")
    exit(1)

process = subprocess.run(f"docker start {CONTAINER}", shell=True, capture_output=True, text=True)
if process.returncode != 0:
    print(f"Failed to start {CONTAINER}")
    exit(1)

if args.clean:
    command = "rm -rf Rivanna2/cmake_build/"
    process = subprocess.run(f"docker exec -t {CONTAINER} /bin/bash -c \"{command}\"", shell=True, capture_output=True, text=True)
    if process.returncode != 0:
        print("Failed to clean the build.")
        exit(1)

command = f"cd Rivanna2/ && ./compile.sh {' '.join(args.args)}"
compile_process = subprocess.run(f"docker exec -t {CONTAINER} /bin/bash -c \"{command}\"", shell=True, capture_output=args.silent, text=True)
if args.silent and compile_process.returncode != 0:
    print("Compilation failed.")

process = subprocess.run(f" docker stop {CONTAINER}", shell=True, capture_output=True, text=True)
if process.returncode != 0:
    print(f"Failed to stop {CONTAINER}")
    exit(1)

exit(compile_process.returncode)