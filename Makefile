all:
	gcc -Wall --pedantic src/*.c -g -o linux.exe

test: all
	./linux.exe graf.csrrg
