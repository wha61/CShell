all: cshell

cshell : cshell.c
	gcc -g -Wall -o cshell cshell.c
	
clean: 
	rm cshell 