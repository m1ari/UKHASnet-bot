#ifndef connection_h_
#define connection_h_

#include <string>

namespace UKHASnet {

class Connection {
	private:
		pthread_t threadid;
		bool run;
		bool connected;
		int sockfd;
		//struct sockaddr_in dest;
		//struct hostent *he;
		std::string server;
		std::string nick;
		std::string username;
		std::string realname;
		std::string password;
		// list of channels

		static void* entryPoint(void *pthis);
		void mainLoop();
		void sendBuffer(const char* buf, size_t length);
		void sendNick();
		void sendUser();
		void sendPong(std::string req);
		void sendJoin(std::string chan);
		void sendMsg(std::string dest, std::string msg);
	public:
		Connection();
		~Connection();
		void setServer(std::string in);
		void setNick(std::string in);
		void setUser(std::string in);
		void setPassword(std::string in);
		int connect();	// Starts new thread
		void disconnect();
		void join(std::string channel);
		void part(std::string channel);
		void part(std::string channel, std::string msg);
	protected:

};

}
#endif
