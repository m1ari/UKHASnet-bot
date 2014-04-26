// #include <syslog.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <map>
#include "irc/connection.h"
#include "irc/server.h"
#include "config.h"


using namespace UKHASnet;

int main(int argc, char **argv){
	fprintf(stderr,"Starting with PID %d\n", getpid());
	//irc::Connection freenode;
	Config conf;

	conf.setFile("config.json");
	conf.loadConfig();

	std::list<std::string> ircServers;
	ircServers = conf.getIRCServerList();

	std::map<std::string, irc::Connection> ircConnections;

	for (std::list<std::string>::iterator it = ircServers.begin(); it != ircServers.end(); it++){
		irc::Server s;
		s=conf.getIrcServer(*it);
		std::cout << "Got Server: " << s.getServer() << "(" << s.getNick() << "!" << s.getUser() << std::endl;
		//ircConnections[*it]=new irc::Connection;
		ircConnections[*it].setServer(s.getServer());
		ircConnections[*it].setNick(s.getNick());
		ircConnections[*it].setUser(s.getUser());
		ircConnections[*it].connect();
	}

// map / set to store connections
	//std::cout << "Server: " << conf.getString(2, "irc", "server") << std::endl;
	//std::cout << "Channels: " << conf.getString(2, "irc", "channels") << std::endl;

	//freenode.setServer("gateway.yapd.net");
	//freenode.setNick("HasBot");
	//freenode.setUser("HasBot");

	//freenode.connect();

	//sleep(5);
	//freenode.join("#foo");
	//sleep(5);
	//freenode.part("#foo");
	//sleep(5);
	//freenode.join("#foo");
	//sleep(5);
	//freenode.part("#foo", "wibble");
	sleep(5);



	/*
	for (int i=0; i<600; i++){
		sleep(1);
	}
	*/
	//freenode.disconnect();

}
