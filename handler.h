#ifndef handler_h_
#define handler_h_

#include <queue>
#include <map>
#include <pthread.h>
#include "irc/connection.h"
#include "irc/message.h"
#include "logger.h"

namespace UKHASnet {

class Handler {
	private:
		static pthread_mutex_t mutex;
		pthread_t threadid;
		static std::queue<irc::Message> messages;
		static bool run;
		irc::Connection *server;

		std::map<std::string, Logger> logs; // string made up as servername + channel

		// Thread functions
                static void* entryPoint(void *pthis);
		void mainLoop();
	public:
		Handler();			// Used by the main process
		Handler(void *server);		// Used by a server process
		~Handler();

		// Thread functions
		void start();
		void stop();

		// Functions for server process
		void addMessage(irc::Message msg);
	protected:

};

}
#endif
