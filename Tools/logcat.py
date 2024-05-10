import subprocess
import argparse

parser = argparse.ArgumentParser()
args = parser.parse_args()


def run_adb_logcat():
    try:
        # Run adb logcat command
        command = ['adb', 'logcat', '-v', 'threadtime']
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        while True:
            line = process.stdout.readline().decode('utf-8')
            line = line.rstrip('\n')
            line = line.rstrip('\r')
            if not line:
                break
            if " V " in line:
                print(f"\033[97m{line}\033[0m")
            elif " D " in line:
                print(f"\033[34m{line}\033[0m")
            elif " I " in line:
                print(f"\033[32m{line}\033[0m")
            elif " W " in line:
                print(f"\033[33m{line}\033[0m")
            elif " E " in line:
                print(f"\033[31m{line}\033[0m")
            elif " F " in line:
                print(f"\033[35m{line}\033[0m")
            else:
                print(line, end='')
    except subprocess.CalledProcessError as e:
        print(f"Error running adb logcat: {e}")


if __name__ == "__main__":
    while True:
        run_adb_logcat()
