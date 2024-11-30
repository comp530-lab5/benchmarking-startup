import subprocess
"""

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

"""
import math

chunk_size = (math.floor((((math.pow(2, 20)*100) - math.pow(2, 12)) / 16) / math.pow(2, 12))) * math.pow(2, 12);
extension = "test"

## TEST 1, HDD
size = 4096;
for i in range(16):
    for i in range(10):
        command = f"./benchmark_tests -d1 -t1 -b{size} -e{extension}"
        subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
   size += chunk_size

size = math.pow(2, 20) * 100;
command = f"./benchmark_tests -d1 -t1 -b{size} -e{extension}"
subprocess.run(command, shell=True, check=True, text=True, capture_output=True)



## TEST 1, SSD
size = 4096;
for i in range(16):
    for i in range(10):
        command = f"./benchmark_tests -d2 -t1 -b{size} -e{extension}"
        subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
   size += chunk_size

size = math.pow(2, 20) * 100;
command = f"./benchmark_tests -d2 -t1 -b{size} -e{extension}"
subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
