#!/usr/bin/python3
from pathlib import Path
from subprocess import run, CompletedProcess
from sys import argv, exit

# used python instead of my bash script so that i can run it on windows too :)
# however, i will probably rewrite this in C later or make a small build tool myself
# this is just a temporary fix

CC = "gcc"
BUILD_DIR = Path("build")
CFLAGS = [ "-std=c99", "-Wall" ]
LINKFLAGS = [ "-lpthread", "-lm", "-lcurl" ]

def log(level: str, message: str) -> None:
    print(f"[{level.upper()}]: {message}")

def cmd(*args: str) -> CompletedProcess[bytes]:
    log("CMD", " ".join(args))
    return run(args, check=True)

def run_c(source_file: str, *args: str) -> None:
    source = Path(source_file).absolute()
    exe = BUILD_DIR / source.with_suffix("").name
    process = cmd(CC, source.as_posix(), "-o", exe.as_posix(), *CFLAGS, *LINKFLAGS)
    if process.returncode != 0:
        print(f"Error compiling {source_file}")
        exit(1)

    process = cmd(exe.as_posix(), *args)
    if process.returncode != 0:
        log("ERROR", f"Program exited with {process.returncode}")
        exit(1)

    log("INFO", f"Program {exe.as_posix()} executed successfully")
    process = cmd("rm", "-f", exe.as_posix())
    if process.returncode != 0:
        log("ERROR", f"Failed to remove executable {exe.as_posix()}")
        exit(1)

def run_tests() -> None:
    run_c("./src/testing/yoru_tests.c")
    
def run_demo(number: str) -> None:
    run_c(f"./src/_examples/{number}.c")

def print_usage() -> None:
    print("Usage: python yoru.py <command> [option]")
    print("Commands:")
    print("  test           - Run all tests")
    print("  demo <number>  - Run a demo")

if __name__ == "__main__":
    BUILD_DIR.mkdir(exist_ok=True)
    
    if len(argv) < 2:
        print_usage()
        exit(1)

    command = argv[1]
    if command == "test":
        run_tests()

    elif command == "demo":
        if len(argv) < 3:
            log("ERROR", "Demo number is required")
            print_usage()
            exit(1)

        demo_number = argv[2]
        run_demo(demo_number)
    
    else:
        log("ERROR", f"Unknown command: {command}")
        print_usage()
