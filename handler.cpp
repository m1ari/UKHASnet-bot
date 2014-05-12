#include <queue>
#include <pthread.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "handler.h"
#include "irc/server.h"
#include "irc/message.h"

namespace UKHASnet {
	// Static vars
	pthread_mutex_t Handler::mutex=PTHREAD_MUTEX_INITIALIZER;
	std::queue<irc::Message> Handler::messages;
	bool Handler::run=false;

	Handler::Handler(){
		server=NULL;
	}

	Handler::Handler(void *s){
		server = (irc::Connection*)s;
		// Increment server ref count ?
	}

	Handler::~Handler(){
		// Decrement server ref count ?

	}

	void* Handler::entryPoint(void *pthis){
		Handler *self= (Handler*)pthis;
		self->mainLoop();
		pthread_exit(0);
	}

	void Handler::mainLoop(){
		printf("Handler process started\n");
		irc::Message msg;
		while (run){
			pthread_mutex_lock(&mutex);
			if (messages.empty()){
				//std::cout << "Handler: No Messages" << std::endl;
				pthread_mutex_unlock(&mutex);
				sleep(1);
			} else {
				// TODO need to check that msg will be a copy otherwise this isn't safe
				msg=messages.front();
				messages.pop();
				pthread_mutex_unlock(&mutex);

				// Log
				std::string logdest;
				std::string logfile;
				if (msg.getServer()->isChannel(msg.getDest())){
					logdest=msg.getServer()->getServerName() + "/" +  msg.getDest();
					logfile=msg.getDest();
				} else {
					logdest=msg.getServer()->getServerName() + "/" +  msg.getNick();
					logfile=msg.getNick();
				}

				if (logs.find(logdest) == logs.end()) {	// Doesn't exist
					logs[logdest].setPath("log/public/" + logdest);
					logs[logdest].setName(logfile);
					logs["~" + logdest].setPath("log/private/" + logdest);
					logs["~" + logdest].setName(logfile);
				}

				if (msg.getText().find("!h ") == 0){	// Log Hidden
					if (msg.getText().find("ACTION") == 0){
						logs["~" + logdest].writeLog(" * " + msg.getNick() + " " + msg.getText());
					} else {
						logs["~" + logdest].writeLog("<" + msg.getNick() + "> " + msg.getText());
					}
				} else {				// Log Normal
					if (msg.getText().find("ACTION") == 0){
						logs[logdest].writeLog(" *" + msg.getNick() + " " + msg.getText());
						logs["~" + logdest].writeLog(" *" + msg.getNick() + " " + msg.getText());
					} else {
						logs[logdest].writeLog("<" + msg.getNick() + "> " + msg.getText());
						logs["~" + logdest].writeLog("<" + msg.getNick() + "> " + msg.getText());
					}
				}

				// TODO Ideally this should be managed via a map created by functions registering themselves
				if (msg.getText().find("!help") == 0){
					std::cout << "Handler: Got help request" << std::endl;
				} else if (msg.getText().find("!node") == 0){
					std::cout << "Handler: Got Node request" << std::endl;
				} else if (msg.getText().find("!follow") == 0){
					std::cout << "Handler: Got Follow request" << std::endl;
				} else if (msg.getText().find("!admin") == 0){
					std::cout << "Handler: Got Admin request" << std::endl;
					// Todo Check authorised user (from config ??)
					// Get 2nd keyword
					// listserv	(lists known servers and state)
					// connect <serv>
					// disco <serv>
				} else {
					std::cout << "Handler: Processing " << msg.getNick() << ": " << msg.getText() << std::endl;
				}
			}
		}
		printf("Handler prcess ending\n");
	}

	void Handler::start(){
		if (server!=NULL){
			fprintf(stderr, "Error: Trying to start handler thread with server defined\n");
			return;
			// Only start the thread if this instance doesn't have a server ID set
		}

		if (run==true){
			fprintf(stderr, "Error: Handler thread already running\n");
			return;
		}

		run=true;
		pthread_create(&threadid,NULL,&Handler::entryPoint,this);
	}

	void Handler::stop(){
		run=false;
		pthread_join(threadid,NULL);
	}

	void Handler::addMessage(irc::Message msg){
		if (server!=NULL){
			// set the msg server object if it's not set
			if (!msg.hasServer()){
				msg.setServer(server);
			}
			pthread_mutex_lock(&mutex);
			messages.push(msg);
			pthread_mutex_unlock(&mutex);
		} else {
			// server not set - we shouldn't add this message
		}
	}
}
