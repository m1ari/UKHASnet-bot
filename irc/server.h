#ifndef irc_server_h_
#define irc_server_h_

#include <string>
#include <set>
// #include "channel.h"

namespace UKHASnet {
namespace irc {

class Server {
	private:
		std::string name;	// Name of encompassing object
		std::string hostname;
		std::string nick;
		std::string username;
		std::string realname;
		std::string password;
		bool log;
		bool connect;

		std::set<std::string> channels;	// Change to use Channel object later

	public:
		Server();
		~Server();

		void setName(std::string name);
		void setServer(std::string hostname);
		void setNick(std::string nick);
		void setUser(std::string username);
		void setRealname(std::string name);
		void setPass(std::string password);
		void setLog(bool log);
		void setConnect(bool connect);

		std::string getName() const;
		std::string getServer() const;
		std::string getNick() const;
		std::string getUser() const;
		std::string getRealname() const;
		std::string getPass() const;
		bool getLog() const;
		bool getConnect() const;

		void addChannel(std::string c);
		void delChannel(std::string c);
		size_t getNumChannels() const;
		//Channel getChannel(int n) const;
		bool isChannel(std::string channel) const;

	protected:

};

}
}
#endif
