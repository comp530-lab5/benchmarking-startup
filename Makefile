all: benchmark_tests

CFLAGS += -g -Wall -Werror -D_GNU_SOURCE 

%.o: %.c *.h
	gcc $(CFLAGS) -c -o $@ $<

benchmark_tests: benchmark_tests.o
	gcc $(CFLAGS) -o benchmark_tests benchmark_tests.o
