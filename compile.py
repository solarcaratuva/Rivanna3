import subprocess


CONTAINER = "Rivanna3_compile"

process = subprocess.run("docker --help", shell=True, capture_output=True, text=True)
if process.returncode != 0:
    print("Docker is not running. Please start Docker and try again.")
    exit(1)

process = subprocess.run(f"docker start {CONTAINER}", shell=True, capture_output=True, text=True)
if process.returncode != 0:
    print(f"Failed to start {CONTAINER}")
    exit(1)

process = subprocess.run(f"docker exec {CONTAINER} 'cd Rivanna2/ && ./compile.sh'", shell=True, capture_output=False, text=True)
if process.returncode != 0:
    print(f"Failed to run command in {CONTAINER}")
    exit(1)

process = subprocess.run(f" docker stop {CONTAINER}", shell=True, capture_output=True, text=True)
if process.returncode != 0:
    print(f"Failed to stop {CONTAINER}")
    exit(1)