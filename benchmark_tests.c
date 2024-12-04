// 730473910
// 730567999

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <math.h>

enum Test_type {
    IO_SIZE_R,
    IO_SIZE_W,
    IO_STRIDE_R,
    IO_STRIDE_W,
    RANDOM_IO_R,
    RANDOM_IO_W
};

enum Device {
    HDD,
    SSD
};

// used to specify file where results are output to
static char rfile_extension[30] = "default";

// Function prototypes
void help();
void write_results(enum Test_type test_type, enum Device device, int block_size, int stride_size, double throughput); 
void io_size_test_R(const char *device, size_t block_size);
void io_size_test_W(const char *device, size_t block_size);
void io_stride_test_R(const char *device, size_t block_size, size_t stride_size);
void io_stride_test_W(const char *device, size_t block_size, size_t stride_size);
void io_random_test_R(const char *device, size_t block_size);
void io_random_test_W(const char *device, size_t block_size);

void help() {
    printf("Benchmarking tests\n");
    printf("Options:\n");
    printf("\t-t - test type - I/0 Size R/W (1/2), I/O Stride R/W (3/4), Random I/O R/W (5/6)\n");
    printf("\t-d - device - HDD (1), SSD (2)\n");
    printf("\t-b - block size in bytes (e.g., 4096 for 4KB)\n");
    printf("\t-s - stride size in bytes (optional, only for I/O Stride test)\n");
    printf("\t-e - results file extension name\n");
    printf("\t-h - help\n");
}

// Test for I/O Size, Read
/* Sequentially read a range of logical blocks up to 1 GB, in different granularities.
 * Measure the throughput (i.e., 1 GB divided by the execution time of the reads) */
// device - 0 for HDD, 1 for SSD
// block_size - size of chunks to write to disk at a time
void io_size_test_R(const char *device, size_t block_size) {
    // get an integer representation of device (0 for hdd, 1 for ssd)
    int passed_device = 1;
    if(strcmp("/dev/sda2", device)) { // device is hdd
        passed_device = 0;
    }

    int fd = open(device, O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(4096, block_size); // Allocates a block worth of memory 
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }
    memset(buffer, 1, block_size); // memset to ensure buffer is cached and doesn't bottleneck test

    size_t disk_size;
    disk_size = (passed_device == 0) ? (1L * 1024 * 1024 * 1024 * 2) : (1L * 1024 * 1024 * 1024 * 2);

    size_t total_bytes = 1L * 1024 * 1024 * 1024; // total memory is 1 GB
    size_t bytes_read = 0; // number of read bytes
    off_t offset = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (bytes_read < total_bytes) {
        ssize_t bytes = pread(fd, buffer, block_size, offset);
        if (bytes < 0) {
            perror("Read failed");
            break;
        }
        bytes_read += bytes;
        offset += bytes;
        offset %= disk_size;
    }

    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6; // time in seconds
    double throughput = (double)bytes_read / elapsed / (1024 * 1024); // MB/s
    
    printf("I/O Size Test: Device: %s, Block size: %zu bytes, Throughput: %.2f MB/s\n", 
    device, block_size, throughput);

    free(buffer);
    close(fd);

    write_results(IO_SIZE_R, passed_device, block_size, 0, throughput);
    return;
}




// Test for I/O Size, Write
/* Sequentially write a range of logical blocks up to 1 GB, in different granularities.
 * Measure the throughput (i.e., 1 GB divided by the execution time of the writes) */
// device - 0 for HDD, 1 for SSD
// block_size - size of chunks to write to disk at a time
void io_size_test_W(const char *device, size_t block_size) {
    // get an integer representation of device (0 for hdd, 1 for ssd)
    int passed_device = 1;
    if(strcmp("/dev/sda2", device)) { // device is hdd
        passed_device = 0;
    }

    int fd = open(device, O_WRONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(4096, block_size); // Allocates a block worth of memory 
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }
    memset(buffer, 0, block_size); // fill buffer with zeros to start
                                   
    size_t disk_size;
    disk_size = (passed_device == 0) ? (1L * 1024 * 1024 * 1024 * 2) : (1L * 1024 * 1024 * 1024 * 2);

    size_t total_bytes = 1L * 1024 * 1024 * 1024; // total memory is 1 GB
    size_t written = 0; // number of written bytes
    off_t offset = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (written < total_bytes) {
        ssize_t bytes = pwrite(fd, buffer, block_size, offset);
        if (bytes < 0) {
            perror("Write failed");
            break;
        }
        written += bytes;
        offset += bytes;
        offset %= disk_size;
    }

    fsync(fd);
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6; // time in seconds
    double throughput = (double)written / elapsed / (1024 * 1024); // MB/s
    
    printf("I/O Size Test: Device: %s, Block size: %zu bytes, Throughput: %.2f MB/s\n", 
    device, block_size, throughput);

    free(buffer);
    close(fd);

    write_results(IO_SIZE_W, passed_device, block_size, 0, throughput);
    return;
}



// Test for I/O Stride, read
/* Same as previous test, leaves a configurable amount of space between each I/O request in the sequential read */
void io_stride_test_R(const char *device, size_t block_size, size_t stride_size) {
    // get an integer representation of device (0 for hdd, 1 for ssd)
    int passed_device = 1;
    if(strcmp("/dev/sda2", device)) { // device is hdd
        passed_device = 0;
    }

    int fd = open(device, O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(4096, block_size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }
    memset(buffer, 1, block_size); // ensure buffer is cached

    size_t disk_size;
    disk_size = (passed_device == 0) ? (1L * 1024 * 1024 * 1024 * 2) : (1L * 1024 * 1024 * 1024 * 2);

    size_t total_bytes = 1L * 1024 * 1024 * 1024;
    size_t bytes_traversed = 0;
    size_t bytes_read = 0;
    off_t offset = 0; // keeps track of the current logical block address for the next write

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (bytes_traversed < total_bytes) {
        ssize_t bytes = pread(fd, buffer, block_size, offset);
        if (bytes < 0) {
            perror("Read failed");
            break;
        }
        bytes_traversed += bytes + stride_size; // this way, we only read up to total_bytes and not more than that
        bytes_read += bytes;
        offset += bytes_traversed; // advance by block size + stride
        offset %= disk_size;
    }

    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    double throughput = (double)bytes_read / elapsed / (1024 * 1024); // MB/s

    printf("I/O Stride Test: Device: %s, Block size: %zu bytes, Stride: %zu bytes, Throughput: %.2f MB/s\n", device, block_size, stride_size, throughput);

    free(buffer);
    close(fd);

    write_results(IO_STRIDE_R, passed_device, block_size, stride_size, throughput);
}



// Test for I/O Stride, write
/* Same as previous test, leaves a configurable amount of space between each I/O request in the sequential write */
void io_stride_test_W(const char *device, size_t block_size, size_t stride_size) {
    // get an integer representation of device (0 for hdd, 1 for ssd)
    int passed_device = 1;
    if(strcmp("/dev/sda2", device)) { // device is hdd
        passed_device = 0;
    }

    int fd = open(device, O_WRONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(4096, block_size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }
    memset(buffer, 0, block_size);

    size_t disk_size;
    disk_size = (passed_device == 0) ? (1L * 1024 * 1024 * 1024 * 2) : (1L * 1024 * 1024 * 1024 * 2);

    size_t total_bytes = 1L * 1024 * 1024 * 1024;
    size_t written = 0;
    size_t bytes_traversed;
    off_t offset = 0; // keeps track of the current logical block address for the next write

    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (bytes_traversed < total_bytes) {
        ssize_t bytes = pwrite(fd, buffer, block_size, offset);
        if (bytes < 0) {
            perror("Write failed");
            break;
        }
        bytes_traversed += bytes + stride_size; // this way we only write up to total_bytes, not more
        written += bytes;
        offset += bytes_traversed; // advance by block size + stride
        offset %= disk_size;
    }

    fsync(fd);
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    double throughput = (double)written / elapsed / (1024 * 1024); // MB/s

    printf("I/O Stride Test: Device: %s, Block size: %zu bytes, Stride: %zu bytes, Throughput: %.2f MB/s\n", device, block_size, stride_size, throughput);

    free(buffer);
    close(fd);

    write_results(IO_STRIDE_W, passed_device, block_size, stride_size, throughput);
}



// Test for Random I/O, read
/* Same as previous test, leaves a random amount of space between each I/O request in the sequential read instead of configurable
 * Random I/O access within a 1 GB swath at block_size granularity */
void io_random_test_R(const char *device, size_t block_size) {
    // get an integer representation of device (0 for hdd, 1 for ssd)
    int passed_device = 1;
    if(strcmp("/dev/sda2", device)) { // device is hdd
        passed_device = 0;
    }

    int fd = open(device, O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(4096, block_size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }
    memset(buffer, 1, block_size); // ensure buffer is cached

    size_t disk_size;
    disk_size = (passed_device == 0) ? (1L * 1024 * 1024 * 1024 * 2) : (1L * 1024 * 1024 * 1024 * 2);

    size_t total_bytes = 1L * 1024 * 1024 * 1024;
    size_t bytes_traversed = 0;
    size_t bytes_read = 0;
    
    // generate an array of offset values based on smallest block size, loop through them to simulate randomness
    // we need array to be max if we had writes of 4KB and offsets of 4KB. 4KB - 2^12 bytes, 8KB 2^13 bytes
    // actually, just loop through array. 50k is too big
    off_t offset_pool[4096];

    // populate offset_pool, units are bytes (first entry is 4KB)
    off_t available_sizes[16] = {pow(2, 12), pow(2, 13), pow(2, 14), pow(2, 15), 
        pow(2, 16), pow(2, 17), pow(2, 18), pow(2, 19), 
        pow(2, 20), pow(2, 20)*5, pow(2, 20)*10, pow(2, 20)*15,
        pow(2, 20)*25, pow(2, 20)*50, pow(2, 20)*75, pow(2, 20)*100};
    for(int i = 0; i < 4096; i++) {
        int r = rand() % 16;
        offset_pool[i] = available_sizes[r];
    }

    int i = 0;
    off_t stride_size = 0;
    off_t offset = stride_size;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    /* stride_size represents the current random stride
     * offset represents the total amount we want to offset into the file
     */
    while (bytes_traversed < total_bytes) {
        ssize_t bytes = pread(fd, buffer, block_size, offset);
        if (bytes < 0) {
            perror("Read failed");
            break;
        }
        bytes_traversed += bytes + stride_size; // this way, we only read up to total_bytes and not more than that
        bytes_read += bytes;
        offset += bytes_traversed; // advance by block size + stride
        offset = offset % disk_size;
        stride_size = offset_pool[i];
        i++;
        if(i == 4096) { i = 0; }
    }

    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    double throughput = (double)bytes_read / elapsed / (1024 * 1024); // MB/s

    printf("I/O Stride Test: Device: %s, Block size: %zu bytes, Throughput: %.2f MB/s\n", device, block_size, throughput);

    free(buffer);
    close(fd);

    write_results(RANDOM_IO_R, passed_device, block_size, 0, throughput);
    return;
}

// Test for Random I/O, write
/* Same as previous test, leaves a random amount of space between each I/O request in the sequential write instead of configurable
 * Random I/O access within a 1 GB swath at block_size granularity */
void io_random_test_W(const char *device, size_t block_size) {
    // get an integer representation of device (0 for hdd, 1 for ssd)
    int passed_device = 1;
    if(strcmp("/dev/sda2", device)) { // device is hdd
        passed_device = 0;
    }

    int fd = open(device, O_WRONLY | O_DIRECT);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    void *buffer = aligned_alloc(4096, block_size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        close(fd);
        return;
    }
    memset(buffer, 0, block_size);

    size_t disk_size;
    disk_size = (passed_device == 0) ? (1L * 1024 * 1024 * 1024 * 2) : (1L * 1024 * 1024 * 1024 * 2);

    size_t total_bytes = 1L * 1024 * 1024 * 1024;
    size_t bytes_traversed = 0;
    size_t written = 0;

    // generate an array of offset values based on smallest block size, loop through them to simulate randomness
    // we need array to be max if we had writes of 4KB and offsets of 4KB. 4KB - 2^12 bytes, 8KB 2^13 bytes
    // actually, just loop through array. 50k is too big
    off_t offset_pool[4096];

    // populate offset_pool, units are bytes (first entry is 4KB)
    off_t available_sizes[16] = {pow(2, 12), pow(2, 13), pow(2, 14), pow(2, 15), 
        pow(2, 16), pow(2, 17), pow(2, 18), pow(2, 19), 
        pow(2, 20), pow(2, 20)*5, pow(2, 20)*10, pow(2, 20)*15,
        pow(2, 20)*25, pow(2, 20)*50, pow(2, 20)*75, pow(2, 20)*100};

    for(int i = 0; i < 4096; i++) {
        int r = rand() % 16;
        offset_pool[i] = available_sizes[r];
    }

    int i = 0;
    off_t stride_size = 0;
    off_t offset = stride_size;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    /* stride_size represents the current random stride
     * offset represents the total amount we want to offset into the file 
     */
    while (bytes_traversed < total_bytes) {
        ssize_t bytes = pwrite(fd, buffer, block_size, offset);
        if (bytes < 0) {
            perror("Write failed");
            break;
        }
        bytes_traversed += bytes + stride_size;
        written += bytes; // this way we only write up to total_bytes, not more
        offset += bytes_traversed;
        offset %= disk_size;
        stride_size = offset_pool[i];
        i++;
        if(i == 4096) { i = 0; }
    }

    fsync(fd);
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    double throughput = (double)written / elapsed / (1024 * 1024); // MB/s

    printf("I/O Stride Test: Device: %s, Block size: %zu bytes, Throughput: %.2f MB/s\n", device, block_size, throughput);

    free(buffer);
    close(fd);

    write_results(RANDOM_IO_W, passed_device, block_size, 0, throughput);
    return;
}





// stride_size is 0 for tests other than 2,3
void write_results(enum Test_type test_type, enum Device device, int block_size, int stride_size, double throughput) {
    // construct results file path
    char filepath[80] = "";
    strcat(filepath, "results/test_");
    strcat(filepath, rfile_extension);
    strcat(filepath, ".csv");

    // append to results file
    FILE *pFile;
    pFile = fopen(filepath, "a");
    if (pFile == NULL) {
        perror("Failed to open results file");
        return;
    }

    // "test_type","device","block_size","stride_size","throughput"
    char block_size_string[12];
    char stride_size_string[12];
    char throughput_string[50];
    sprintf(block_size_string, "%d", block_size);
    sprintf(stride_size_string, "%d", stride_size);
    sprintf(throughput_string, "%f", throughput);
    char csv_entry[300];
    csv_entry[0] = '\"';
    csv_entry[1] = '0' + test_type;
    csv_entry[2] = '\"';
    csv_entry[3] = ',';
    csv_entry[4] = '\"';
    csv_entry[5] = '0' + device;
    csv_entry[6] = '\"';
    csv_entry[7] = ',';
    csv_entry[8] = '\"';
    csv_entry[9] = '\0';
    strcat(csv_entry, block_size_string);
    strcat(csv_entry, "\",\"");
    strcat(csv_entry, stride_size_string);
    strcat(csv_entry, "\",\"");
    strcat(csv_entry, throughput_string);
    strcat(csv_entry, "\"\n");

    if(fputs(csv_entry, pFile) == EOF) {
        perror("Failed to write to results file\n");
        fclose(pFile);
        return;
    }

    fclose(pFile);
}



// Main function
int main(int argc, char **argv) {
    enum Test_type test_type = -1;
    enum Device device = -1;
    size_t block_size = 0;
    size_t stride_size = 0;
    int c;

    while ((c = getopt(argc, argv, "t:d:b:s:he:")) != -1) { 
        // t = test, d = device, s = stride size, b = block size
        switch (c) {
            case 'h':
                help();
                return 0;
            case 't':
                test_type = atoi(optarg) - 1;
                if (test_type < 0 || test_type > 6) {
                    printf("Incorrect test type\n");
                    help();
                    return 1;
                }
                break;
            case 'd':
                device = atoi(optarg);
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
            case 'e':
                strncpy(rfile_extension, optarg, sizeof(rfile_extension-1));
                rfile_extension[sizeof(rfile_extension)-1] = '\0';
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

    //const char *device_path = (device == 1) ? "/dev/sda2" : "/dev/sdb1";
    //const char *device_path = (device == 1) ? "/dev/sda2" : "/dev/sda1";
    // 1 for ssd, 0 for hdd in this instance
    const char *device_path = (device == 1) ? "/mnt/c/530-tests/ssd-doc.txt" : "/mnt/h/530-tests/hdd-doc.txt";


    switch (test_type) {
        case IO_SIZE_R:
            io_size_test_R(device_path, block_size);
            break;
        case IO_SIZE_W:
            io_size_test_W(device_path, block_size);
            break;
        case IO_STRIDE_R:
            if (stride_size == 0) {
                printf("Stride size required for I/O Stride test\n");
                help();
                return 1;
            }
            io_stride_test_R(device_path, block_size, stride_size);
            break;
        case IO_STRIDE_W:
            if (stride_size == 0) {
                printf("Stride size required for I/O Stride test\n");
                help();
                return 1;
            }
            io_stride_test_W(device_path, block_size, stride_size);
            break;
        case RANDOM_IO_R:
            io_random_test_R(device_path, block_size);
            break;
        case RANDOM_IO_W:
            io_random_test_W(device_path, block_size);
            break;
        default:
            printf("Test type not yet implemented - should never happen\n");
            break;
    }

    return 0;
}
