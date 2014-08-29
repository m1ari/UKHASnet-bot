CFLAGS=-O2 -g -Wall
#  -std=gnu++11 

CC=g++

LIBS=-lpthread -ljansson -lpqxx -lpq
	# -lncurses

OBJS=main.o config.o handler.o logger.o
DB_OBJS=db/database.o db/nodes.o
IRC_OBJS=irc/connection.o irc/server.o irc/message.o irc/channel.o
# irc/member.o
TARGETS=bot

all: $(TARGETS)

bot: $(OBJS) $(IRC_OBJS) $(DB_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp %.h Makefile
	$(CXX) -c $(CFLAGS) -o $@ $<

$(IRC_OBJS): %.o: %.cpp %.h Makefile
	$(CXX) -c $(CFLAGS) -o $@ $<

$(OBJS): %.o: %.cpp %.h Makefile
	$(CXX) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm $(OBJS) $(IRC_OBJS) $(DB_OBJS) $(TARGETS)
