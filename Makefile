all:
	gcc -Wall --pedantic src/*.c -o linux.exe

test: all
	./linux.exe graf.csrrg
