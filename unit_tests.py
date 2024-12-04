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
test_run = "preliminary"

devices = [1, 2]
"""
tests_1 = [1, 2, 5, 6]
for test in tests_1:
	for device in devices:
		command = f"./benchmark_tests -d{device} -t{test} -b4096 -e{test_run}"
		subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
		command = f"./benchmark_tests -d{device} -t{test} -b131072 -e{test_run}"
		subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
"""
tests_2 = [3, 4]
for test in tests_2:
	for device in devices:
		command = f"./benchmark_tests -d{device} -t{test} -b4096 -s4096 -e{test_run}"
		subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
		command = f"./benchmark_tests -d{device} -t{test} -b131072 -s131072 -e{test_run}"
		subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
