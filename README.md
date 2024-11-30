# benchmarking-startup
Super legit and accepting seed funding RIGHT NOW!!!!!

TODO:
understanding:
ask about aligned_alloc

semantics:
maybe rename block_size, any relation to block on disk?

actual fixes:
O_DIRECt - have to be aligned to disk's block size? sized in multiples of block size? posix_memalign?
Read from /dev/zeroes instead of a buffer?
change throughput calculation for iostride

X fix file name parsing
fix offset for stride and random file writing
make last point happen 10 times
change to 5 instead of 10

style guide



WRITEUP NOTES:
/dev/sbd1 is only 512MB, so we used /dev/sda1, which was a 1GB drive
