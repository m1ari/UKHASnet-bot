#ifndef irc_connection_h_
#define irc_connection_h_

#include <string>
#include <ctime>
#include "server.h"
#include "../logger.h"

namespace UKHASnet {
namespace irc {

class Connection {
	private:
		pthread_t threadid;
		bool run;
		bool connected;
		int sockfd;
		Server s;

		Logger log;

		static void* entryPoint(void *pthis);
		void mainLoop();
		void sendBuffer(const char* buf, size_t length);
		void sendNick();
		void sendUser();
		void sendPong(std::string req);
		void sendJoin(std::string chan);
		void sendPart(std::string chan, std::string msg);
		void sendMsg(std::string dest, std::string msg);
	public:
		Connection();
		~Connection();
		void setServer(Server s);
		void setServer(std::string in);
		void setNick(std::string in);
		void setUser(std::string in);
		void setPassword(std::string in);
		void connect();				// Starts new thread
		void disconnect();			// Stops thread and disconnects from server
		void join(std::string channel);		// Join Channel
		void part(std::string channel, std::string msg="");	// Leave Channel
		bool isConnected() const;		// Are we currently connected to the server
	protected:

};

}
}
#endif
