all: benchmark_tests

CFLAGS += -g -Wall -Werror

%.o: %.c *.h
	gcc $(CFLAGS) -c -o $@ $<

benchmark_tests: benchmark_tests.o
	gcc $(CFLAGS) -o benchmark_tests benchmark_tests.o
