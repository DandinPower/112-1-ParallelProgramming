# Compiler and compilation flags
CC=gcc
CFLAGS=-Wall

all: pi

pi: pi.o
	$(CC) $(CFLAGS) -o pi.out pi.o

pi.o: pi.c 
	$(CC) $(CFLAGS) -c pi.c 

# Profiling targets
gprof: profiling_result
	cat profiling_result

profiling_result: pi_gprof.out
	gprof ./pi_gprof.out gmon.out -b > profiling_result

pi_gprof.out: pi.c 
	$(CC) $(CFLAGS) -pg pi.c -o pi_gprof.out
	./pi_gprof.out

# Performance targets
perf: perf.out 
	sudo perf record -e cpu-cycles  ./perf.out
	sudo perf report

perf.out: pi.c 
	$(CC) $(CFLAGS) -g pi.c -o perf.out

# Clean target
clean:
	rm -f *.o *.out profiling_result *.data *.data.old