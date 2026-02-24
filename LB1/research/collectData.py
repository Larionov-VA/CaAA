import subprocess
import re
import sys


print("Компиляция...", file=sys.stderr)
compile_cmd = [
    "g++",
    "-DSTEPIK=0",
    "-DPRINTINFO=0",
    "-DPRINTTIME=1",
    "-O3",
    "../src/main.cpp",
    "-o",
    "../src/main"
]
result = subprocess.run(compile_cmd, capture_output=True, text=True)
if result.returncode != 0:
    sys.exit(1)

for N in range(2, 51):
    input_data = f"{N}\n0\n"
    try:
        proc = subprocess.run(
            ["../src/main"],
            input=input_data,
            text=True,
            capture_output=True,
            timeout=60
        )
    except subprocess.TimeoutExpired:
        print(f"{N} TIMEOUT", file=sys.stderr)
        continue

    if proc.returncode != 0:
        print(f"{N} ERROR (код возврата {proc.returncode})", file=sys.stderr)
        continue

    match = re.search(r'Время выполнения:\s+([0-9.eE+-]+)\s+секунд', proc.stdout)
    if match:
        t = float(match.group(1))
        print(f"{N} {t:.6f}")
    else:
        print(f"{N} ERROR (время не найдено)", file=sys.stderr)