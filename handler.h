#ifndef handler_h_
#define handler_h_

#include <list>
#include <pthread.h>
#include "irc/connection.h"
#include "irc/message.h"

namespace UKHASnet {

class Handler {
	private:
		static pthread_mutex_t mutex;
		pthread_t threadid;
		static std::list<irc::Message> messages;
		static bool run;
		irc::Connection *server;

		// Tread functions
                static void* entryPoint(void *pthis);
		void mainLoop();
	public:
		Handler();
		Handler(irc::Connection *server);
		~Handler();

		// Tread functions
		void start();
		void stop();

		void addMessage(irc::Message mesg);
	protected:

};

}
#endif
