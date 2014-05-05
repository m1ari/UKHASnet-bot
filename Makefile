CFLAGS=-O2 -g -Wall
#  -std=gnu++11 

CC=g++

LIBS=-lpthread -ljansson

OBJS=main.o config.o handler.o
IRC_OBJS=irc/connection.o irc/server.o irc/message.o
# irc/member.o irc/channel.o
TARGETS=bot

all: $(TARGETS)

bot: $(OBJS) $(IRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(IRC_OBJS): %.o: %.cpp %.h Makefile
	$(CXX) -c $(CFLAGS) -o $@ $<

$(OBJS): %.o: %.cpp %.h Makefile
	$(CXX) -c $(CFLAGS) -o $@ $<


.PHONY: clean
clean:
	rm irc/*.o *.o $(TARGETS)
