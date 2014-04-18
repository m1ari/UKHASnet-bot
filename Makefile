CFLAGS=-O2 -g -Wall
CC=g++

LIBS=-lpthread -ljansson

OBJS=connection.o main.o
TARGETS=bot

all: $(TARGETS)

bot: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cxx %.h Makefile debug.h
	$(CXX) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm *.o $(TARGETS)
