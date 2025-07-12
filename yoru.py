#!/usr/bin/python3
from pathlib import Path
from subprocess import run, CompletedProcess
from sys import argv, exit, platform

# used python instead of my bash script so that i can run it on windows too :)
# however, i will probably rewrite this in C later or make a small build tool myself
# this is just a temporary fix

CC = "gcc"
BUILD_DIR = Path("build")
DEMO_DIR = Path("./src/_examples")
CFLAGS = [ "-std=c99", "-Wall" ]

POSIX_LINKFLAGS = [ "-lpthread", "-lm", "-lcurl" ]
WINDOWS_LINKFLAGS = [ "-lm", ]
LINKFLAGS = POSIX_LINKFLAGS if platform == "linux" else WINDOWS_LINKFLAGS

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

    match platform:
        case "linux":
            process = cmd("rm", "-f", exe.as_posix())
        case "win32":
            process = cmd("cmd", "/C", "del", "/F", "/Q", f"{str(exe.absolute())}.exe")
        case _:
            log("ERROR", f"Cannot automatically remove executable on platform {platform}.")
            process.returncode = 1
            
    if process.returncode != 0:
        log("ERROR", f"Failed to remove executable {exe.as_posix()}. You can remove it manually. ")
        exit(1)

    log("INFO", f"Executable {exe.as_posix()} removed successfully")

def run_tests() -> None:
    run_c("./src/testing/yoru_tests.c")
    
def show_demos() -> None:
    print("Available demos:")
    for demo in DEMO_DIR.glob("*.c"):
        print(f"  - {demo.stem}")

def run_demo(demo: str) -> None:
    demo_file = DEMO_DIR / f"{demo}.c"
    if not demo_file.exists():
        log("ERROR", f"Demo {demo} does not exist.")
        print_usage()
        exit(1)
    run_c(demo_file.as_posix())

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
            log("ERROR", "Demo name is required")
            print_usage()
            exit(1)

        arg = argv[2]
        if arg == "--list" or arg == "-l":
            show_demos()
            exit(0)
        run_demo(arg)
    
    else:
        log("ERROR", f"Unknown command: {command}")
        print_usage()
