// #include <syslog.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "irc/connection.h"
#include "irc/server.h"
#include "config.h"


using namespace UKHASnet;

int main(int argc, char **argv){
	fprintf(stderr,"Starting with PID %d\n", getpid());
	irc::Connection freenode;
	Config conf;

	conf.setFile("config.json");
	conf.loadConfig();

	for (int i=0; i<conf.getNumIrcServers(); i++){
		irc::Server s;
		s=conf.getIrcServer(i);
		std::cout << "Got Server: " << s.getServer() << "(" << s.getNick() << "!" << s.getUser() << std::endl;
	}


	//std::cout << "Server: " << conf.getString(2, "irc", "server") << std::endl;
	//std::cout << "Channels: " << conf.getString(2, "irc", "channels") << std::endl;

	freenode.setServer("gateway.yapd.net");
	freenode.setNick("HasBot");
	freenode.setUser("HasBot");

	freenode.connect();

	sleep(5);
	freenode.join("#foo");
	sleep(5);
	freenode.part("#foo");
	sleep(5);
	freenode.join("#foo");
	sleep(5);
	freenode.part("#foo", "wibble");
	sleep(5);



	/*
	for (int i=0; i<600; i++){
		sleep(1);
	}
	*/
	freenode.disconnect();

}
