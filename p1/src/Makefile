CC = gcc
CFLAGS = -g -Wall

test: readl.o parse_std.o list.o grammar.o

test2: readl.o parse_std.o list.o grammar.o

grammar.o: list.o grammar_connect.o

parse_std.o: grammar.o 

test_stack: stack.o

test_list: list.o

clean:
	rm -f *.o test test2 test_stack test_list
