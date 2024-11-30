import subprocess

# 1 for HDD, 2 for SSD
command = "./benchmark_tests -d1 -t2 -b4096"

try:
    result = subprocess.run(command, shell=True, check=True, text=True, capture_output=True)

    print("Command output:")
    print(f"Error message: {result.stderr}")
    print(result.stdout)
except subprocess.CalledProcessError as e:
    print(f"Command failed with exit code {e.returncode}")
    print(f"Error message: {e.stderr}")
    print(result.stdout)
