// #include <syslog.h>
#include <cstdio>
#include <signal.h>
//#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <map>
#include "irc/connection.h"
#include "irc/server.h"
#include "config.h"
#include "handler.h"
#include "db/database.h"


using namespace UKHASnet;

volatile sig_atomic_t systemloop=true;
void sig_handler(int sig);

int main(int argc, char **argv){
	// Control for main process
	//bool run;
	//run=true;
	signal(SIGHUP,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGUSR1,sig_handler);
	signal(SIGUSR2,sig_handler);

	signal(SIGQUIT,sig_handler);
	signal(SIGINT,sig_handler);
	signal(SIGKILL,sig_handler);

	fprintf(stderr,"Starting with PID %d\n", getpid());

	// Load the config in
	Config conf;
	conf.setFile("config.json");
	conf.loadConfig();

	// Connect to the database
	Database db;
	std::map<std::string, std::string> db_conf = conf.getDBConf();
	db.setDBHost(db_conf["host"]);
	db.setDBUser(db_conf["user"]);
	db.setDBPass(db_conf["password"]);
	db.setDatabase(db_conf["database"]);
	db.connect();


	// Setup handler process
	Handler h;
	// TODO Pass reference for main loop below for handler to use (&bool or local Queue ?)
	h.start();

	// Get list of IRC Servers
	std::list<std::string> ircServers;
	ircServers = conf.getIRCServerList();


	// Iterate over the list of Servers
	std::map<std::string, irc::Connection> ircConnections;
	for (std::list<std::string>::iterator it = ircServers.begin(); it != ircServers.end(); it++){
		// Get Server details from the config file
		irc::Server s;
		s=conf.getIRCServer(*it);
		std::cout << "Got Server: " << s.getServer() << "(" << s.getNick() << "!" << s.getUser() << ")" << std::endl;

		// Tell each connection about the server it connects to
		ircConnections[*it].setServer(s);
		if (s.getConnect()==true){
			// Start new thread and Connect to server
			ircConnections[*it].connect();
			// TODO Once connection process remembers channels
			// and auto connects we won't need to wait here
			while (!ircConnections[*it].isConnected()){
				sleep (1);
			}
		}

		// Get list of channels for the server
		std::list<std::string> channels;
		channels=conf.getIRCChannels(*it);
		for (std::list<std::string>::iterator it2 = channels.begin(); it2 != channels.end(); it2++){
			std::cout << "Joining " << *it2 << std::endl;
			ircConnections[*it].join(*it2);
		}
	}

	while (systemloop){
		// TODO handle keypresses - or just go to background and signals ?
		sleep(1);
	}


	for (std::list<std::string>::iterator it = ircServers.begin(); it != ircServers.end(); it++){
		// TODO Should just tell the servers to disconnect and they'll leave the channels
		std::list<std::string> channels;
		channels=conf.getIRCChannels(*it);
		for (std::list<std::string>::iterator it2 = channels.begin(); it2 != channels.end(); it2++){
			ircConnections[*it].part(*it2,"Who Killed me");
		}
		ircConnections[*it].disconnect();
	}
}

void sig_handler(int sig){
	fprintf(stderr,"Got Signal %d\n",sig);
	switch (sig){
		case SIGHUP:
			// Sig HUP, Do a reload
			fprintf(stderr,"Sig: Got SIGHUP\n");
		break;
		case SIGINT: // 2
			// Interupt (Ctrl c From command line) - Graceful shutdown
			fprintf(stderr,"Sig: Got SIGINT - Shutting down\n");
			systemloop=false;
		break;
		case 15:
			// TERM
			fprintf(stderr,"Sig: Got SIGTERM\n");
		break;
		case 16:
			// USR1
			fprintf(stderr,"Sig: Got SIGUSR1\n");
		break;
		case 17:
			// USR2
			fprintf(stderr,"Sig: Got SIGUSR2\n");
		break;
	}
}


