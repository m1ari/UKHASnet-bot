CFLAGS=-O2 -g -Wall
CC=g++

LIBS=-lpthread -ljansson

OBJS=main.o main.o config.o
IRC_OBJS=irc/connection.o irc/server.o irc/channel.o irc/member.o
TARGETS=bot

all: $(TARGETS)

bot: $(OBJS) $(IRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cxx %.h Makefile debug.h
	$(CXX) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm irc/*.o *.o $(TARGETS)
