// 730473910
// 730567999

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

enum Test_type {
    IO_SIZE,
    IO_STRIDE,
    RANDOM_IO,
    READ_V_WRITE
};

enum Device {
    HDD,
    SSD
};

// Function prototypes
void help();
void io_size_test(const char *device, size_t block_size);
void io_stride_test(const char *device, size_t block_size, size_t stride_size);

void help() {
    printf("Benchmarking tests\n");
    printf("Options:\n");
    printf("\t-t - test type - I/0 Size (1), I/O Stride (2), Random I/O (3), Read vs. Write (4)\n");
    printf("\t-d - device - HDD (1), SSD (2)\n");
    printf("\t-b - block size in bytes (e.g., 4096 for 4KB)\n");
    printf("\t-s - stride size in bytes (optional, only for I/O Stride test)\n");
    printf("\t-h - help\n");
}

void io_size_test(const char *device, size_t block_size) {
    int fd = open(device, O_WRONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(512, block_size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }

    memset(buffer, 0, block_size); // fill buffer with zeros to start
    size_t total_bytes = 1L * 1024 * 1024 * 1024; // total memory is 1 GB
    size_t written = 0; // number of written bytes

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (written < total_bytes) {
        ssize_t bytes = write(fd, buffer, block_size);
        if (bytes < 0) {
            perror("Write failed");
            break;
        }
        written += bytes;
    }

    fsync(fd);
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    double throughput = (double)written / elapsed / (1024 * 1024); // MB/s
    
    printf("I/O Size Test: Device: %s, Block size: %zu bytes, Throughput: %.2f MB/s\n", 
    device, block_size, throughput);

    free(buffer);
    close(fd);
}

// Test for I/O Stride
void io_stride_test(const char *device, size_t block_size, size_t stride_size) {
    int fd = open(device, O_WRONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(512, block_size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }

    memset(buffer, 0, block_size);
    size_t total_bytes = 1L * 1024 * 1024 * 1024;
    size_t written = 0;
    off_t offset = 0; // keeps track of the current logical block address for the next write

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (written < total_bytes) {
        ssize_t bytes = pwrite(fd, buffer, block_size, offset);
        if (bytes < 0) {
            perror("Write failed");
            break;
        }
        written += bytes;
        offset += block_size + stride_size; // advance by block size + stride
    }

    fsync(fd);
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    double throughput = (double)written / elapsed / (1024 * 1024); // MB/s

    printf("I/O Stride Test: Device: %s, Block size: %zu bytes, Stride: %zu bytes, Throughput: %.2f MB/s\n", device, block_size, stride_size, throughput);

    free(buffer);
    close(fd);
}

// Main function
int main(int argc, char **argv) {
    enum Test_type test_type = -1;
    enum Device device = -1;
    size_t block_size = 0;
    size_t stride_size = 0;
    int c;

    while ((c = getopt(argc, argv, "t:d:b:s:h")) != -1) { 
        // t = test, d = device, s = stride size, b = block size
        switch (c) {
            case 'h':
                help();
                return 0;
            case 't':
                test_type = atoi(optarg) - 1;
                if (test_type < 0 || test_type > 3) {
                    printf("Incorrect test type\n");
                    help();
                    return 1;
                }
                break;
            case 'd':
                device = atoi(optarg) - 1;
                if (device != 1 && device != 2) {
                    printf("Incorrect device type\n");
                    help();
                    return 1;
                }
                break;
            case 'b':
                block_size = strtoul(optarg, NULL, 10);
                if (block_size == 0) {
                    printf("Invalid block size\n");
                    help();
                    return 1;
                }
                break;
            case 's':
                stride_size = strtoul(optarg, NULL, 10);
                break;
            default:
                printf("Unknown option.\n");
                help();
                return 1;
        }
    }

    if (test_type == -1 || device == -1 || block_size == 0) {
        printf("Missing required arguments\n");
        help();
        return 1;
    }

    const char *device_path = (device == 1) ? "/dev/sda2" : "/dev/sdb1";

    switch (test_type) {
        case 0:
            io_size_test(device_path, block_size);
            break;
        case 1:
            if (stride_size == 0) {
                printf("Stride size required for I/O Stride test\n");
                help();
                return 1;
            }
            io_stride_test(device_path, block_size, stride_size);
            break;
        default:
            printf("Test type not yet implemented\n");
            break;
    }

    return 0;
}
