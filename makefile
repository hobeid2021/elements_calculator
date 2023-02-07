FILES = main.c tinyexpr.c repl.c
CC=gcc
default:
	$(CC) -c $(FILES)
	$(CC) *.o -o main -lm -lreadline 
clean:
	rm ./*.o
run:
	./main
