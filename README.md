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

style guide



WRITEUP NOTES:
/dev/sbd1 is only 512MB, so we wrote/read to the disk twice to reach 1GB
