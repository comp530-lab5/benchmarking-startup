import subprocess
import math

# Generate block sizes based on powers of 2, from 4 KB (2^12) to 100 MB (2^20 * 100)
block_sizes = [2**i for i in range(12, 21)] + [100 * (2**20)]
extension = "test" # results will be output to test_{extension}.c

devices = [1, 2]
tests_1 = [1, 2, 5, 6]


# I/O Size, Random I/O
for test in tests_1:
    for device in devices:
        for size in block_sizes:
            for j in range(5):  # Repeat the test 5 times for each block size
                command = f"./benchmark_tests -d{device} -t{test} -b{size} -e{extension}"
                subprocess.run(command, shell=True, check=True, text=True, capture_output=True)


tests_2 = [3, 4]
stride_sizes = [2**i for i in range(12, 21)] + [100 * (2**20)]

# I/O Stride
for test in tests_2:
    for device in devices:
        for granularity in block_sizes:  # Granularity is equivalent to block sizes
            for stride in stride_sizes:
                for j in range(5):  # Repeat the test 5 times for each combination
                    command = f"./benchmark_tests -d{device} -t{test} -b{granularity} -s{stride} -e{extension}"
                    subprocess.run(command, shell=True, check=True, text=True, capture_output=True)

