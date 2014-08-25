#include <queue>
#include <pthread.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "handler.h"
#include "db/database.h"
#include "db/nodes.h"
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
		Database db;
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

				if (msg.getText().find("\001ACTION") == 0){		// /me message
					std::string msgtext = msg.getText().substr(8);	// Remove the \001ACTION part
					msgtext.erase(msgtext.find_last_of('\001'),1);	// Remove the trailing \001
					if (msgtext.find("!h ") == 0){		// ACTION !h
						logs["~" + logdest].writeLog(" * " + msg.getNick() + " " + msgtext);
					} else {
						logs["~" + logdest].writeLog(" * " + msg.getNick() + " " + msgtext);
						logs[logdest].writeLog(      " * " + msg.getNick() + " " + msgtext);
					}
				} else {						// normal message
					if (msg.getText().find("!h ") == 0){
						logs["~" + logdest].writeLog("<" + msg.getNick() + "> " + msg.getText());
					} else {
						logs[logdest].writeLog(      "<" + msg.getNick() + "> " + msg.getText());
						logs["~" + logdest].writeLog("<" + msg.getNick() + "> " + msg.getText());
					}
				}

				// TODO Ideally this should be managed via a map created by functions registering themselves
				if (msg.getText().find("!help") == 0){
					std::cout << "Help(" << msg.getNick() << "): " << msg.getText() << std::endl;
				} else if (msg.getText().find("!node ") == 0){
					db::Nodes n;
					std::cout << "Node(" << msg.getNick() << "): " << msg.getText() << std::endl;
					std::string data=msg.getText();
					data.erase(0,6);
					size_t p = data.find_first_of("\r\n ");
					data.erase(p);

					msg.reply(n.getNodeLastPacket(data));
				} else if (msg.getText().find("!follow") == 0){
					std::cout << "Follow(" << msg.getNick() << "): " << msg.getText() << std::endl;
				} else if (msg.getText().find("!seen") == 0){
					std::cout << "Seen(" << msg.getNick() << "): " << msg.getText() << std::endl;
				} else if (msg.getText().find("!tell") == 0){
					std::cout << "Tell(" << msg.getNick() << "): " << msg.getText() << std::endl;
				} else if (msg.getText().find("!pub") == 0){
					msg.reply("hic!",true);
				} else if (msg.getText().find("test") == 0){
					msg.reply(msg.getNick() + ": Achievement unlocked: you have joined #ukhasnet",true);
				} else if (msg.getText().find("!chippy") == 0){
					std::cout << "Chippy(" << msg.getNick() << "): " << msg.getText() << std::endl;
				} else if (msg.getText().find("!msg") == 0){
					// !msg name@gateway Message - Used for the conference badges
					db.sendMessage(msg);
					std::cout << "Msg(" << msg.getNick() << "): " << msg.getText() << std::endl;
				} else if (msg.getText().find("!admin") == 0){
					std::cout << "Admin(" << msg.getNick() << "): " << msg.getText() << std::endl;
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
