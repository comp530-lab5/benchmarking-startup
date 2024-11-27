// 730473910
// 730567999

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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

void help() {
    printf("Benchmarking tests\n");
    printf("Options:\n");
    printf("\t-t - test type - I/0 Size (1), I/O Stride (2), Random I/O (3), Read vs. Write (4)\n");
    printf("\t-d - device - HDD (1), SSD (2)\n");
    printf("\t-h - help\n");
}

int main(int argc, char **argv) {
    enum Test_type test_type;
    enum Device device;
    int c;

    // parse cli arguments for test type and device
    while((c = getopt(argc, argv, "t:d:h")) != -1) {
        switch(c) {
            case 'h':
                help();
                return 0;
            case 't':
                int type = atoi(optarg);
                if(type < 1 || type > 4) {
                    printf("Incorrect test type\n");
                    help();
                    return 1;
                }
                test_type = type-1;
                break;
            case 'd':
                int dev = atoi(optarg);
                if(dev != 1 && dev != 2) {
                    printf("Incorrect device type\n");
                    help();
                    return 1;
                }
                device = dev-1;
                break;
            default:
                printf("Unknown option.\n");
                help();
                return 1;
        }
    }


    // TESTING, WILL REMOVE
    printf("Test type: %d, device type: %d\n", test_type, device);

    return 0;
}
