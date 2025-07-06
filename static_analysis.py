#!/usr/bin/env python3
# MUST INSTALL `cppcheck` TO RUN THIS SCRIPT

import subprocess
import sys

BOARDS = ["PowerBoard", "TelemetryBoard",]
UNIVERSAL_ARGS = "--enable=all --quiet --max-ctu-depth=4 --error-exitcode=1"

YELLOW = "\033[33m"
STD = "\033[0m"


def analyze() -> None:
    exit_code = 0
    for board in BOARDS:
        print(f"{YELLOW}Static Analysis for {board}:{STD}")

        board_specific_args = f"-I {board}/include/ -I {board}/lib/include/"
        command = f"cppcheck {UNIVERSAL_ARGS} {board_specific_args} {board}/src/ {board}/lib/src/"
        process = subprocess.run(command, shell=True, check=False, text=True)
        if process.returncode != 0:
            exit_code = process.returncode

    sys.exit(exit_code)


if __name__ == "__main__":
    analyze()