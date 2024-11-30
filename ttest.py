import subprocess
import math

chunk_size = (math.floor((((math.pow(2, 20)*100) - math.pow(2, 12)) / 16) / math.pow(2, 12))) * math.pow(2, 12);
extension = "test" # results will be output to test_{extension}.c

devices = [1, 2]
tests_1 = [1, 2, 5, 6]


# I/O Size, Random I/O
for test in tests_1:
    for device in devices:
        size = 4096
        for i in range(16):
            for j in range(5):
                command = f"./benchmark_tests -d{device} -t{test} -b{size} -e{extension}"
                subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
           size += chunk_size

        size = math.pow(2, 20) * 100;
        for i in range(5):
            command = f"./benchmark_tests -d{device} -t{test} -b{size} -e{extension}"
            subprocess.run(command, shell=True, check=True, text=True, capture_output=True)



tests_2 = [3, 4]
granularity_sizes = [math.pow(2, 12), math.pow(2, 13), math.pow(2, 14), math.pow(2, 16), math.pow(2, 20), math.pow(2, 20)*10] # in bytes

# I/O Stride
for test in tests_2:
    for device in devices:
        for granularity in granularity_sizes:
            stride = 4096
            for i in range(16):
                for j in range(5):
                    command = f"./benchmark_tests -d{device} -t{test} -b{granularity} -s{stride} -e{extension}"
                    subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
                stride += chunk_size
            stride = math.pow(2, 20) * 100;
            for i in range(5):
                command = f"./benchmark_tests -d{device} -t{test} -b{granularity} -s{stride} -e{extension}"
                subprocess.run(command, shell=True, check=True, text=True, capture_output=True)
