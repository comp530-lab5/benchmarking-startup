# benchmarking-startup Super legit and accepting seed funding RIGHT NOW!!!!!
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
SSD offsets can get weird - might slow things down

UPDATES:
ok, so - the 1gb drive we were using is a HDD, not an SSD which is why the speeds were so similar. I changed our code back to the version that runs the 512M SSD and asked a question on campuswire about it. so i think we just wait on that

finished the testing script - this version will take fucking forever to run. 

left to do - make sure io_stride and random_io work correctly (i'm not sure how pread and pwrite work with the offset thing)
make sure testing script looks right


FROM KAYLA:

Okay, so here I added two scripts- data_visualization.py and t-test_caclulations.py
They do exactly what they say. In data_visualizations, it makes graphs for each of the tests according to the writeup and plops them into /graphs. You can see some basic ones I made with incomplete testing data. When we have the data, put the csv file into /results, and change the path in the script to it. We will need to turn it into an executable tomorrow, per the writeup.
In t-test_calculations.py, it calculates the number of samples needed for each observation based on the student's t-test by reordering the formula used to calculate the margin of error to instead calculate the unknown, n (number of samples). Pretty neat. As for data_visualizations.py, go in and change the path variable to point to whatever we have in /results and it will throw a csv file with it's results in /results. We will use this as our table for the challenge question.

If my code is correct, and we get our ouput, all we should need to do after this is the quick writeup. Bless.