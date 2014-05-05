#include <list>
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include "handler.h"
#include "irc/server.h"
#include "irc/message.h"

namespace UKHASnet {

	// Static vars
	pthread_mutex_t Handler::mutex=PTHREAD_MUTEX_INITIALIZER;
	//std::list<irc::Message> Handler::messages;
	bool Handler::run=false;

	Handler::Handler(){
		server=NULL;
	}

	Handler::Handler(irc::Connection *s){
		server=s;
	}

	Handler::~Handler(){

	}

	void* Handler::entryPoint(void *pthis){
		Handler *self= (Handler*)pthis;
		self->mainLoop();
		pthread_exit(0);
	}

	void Handler::mainLoop(){
		printf("Handler process started\n");
		while (run){
			// if queue empty
			sleep(1);
			// else do matches
		}
		printf("Handler prcess ending\n");
	}

	void Handler::start(){
		if (server!=NULL){
			fprintf(stderr, "Error: Trying to start handler thread with server defined\n");
			return;
			// Only start the tread if this instance doesn't have a server ID set
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
	}


	void Handler::addMessage(irc::Message msg){
	}

}

/*
        private:
                static pthread_mutex_t mutex;
                static std::list<message> messages;
                static bool run=false;
                irc::Connection *server;

*/
