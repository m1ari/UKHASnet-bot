#ifndef irc_server_h_
#define irc_server_h_

#include <string>
#include <map>
#include <list>
#include "channel.h"

namespace UKHASnet {
namespace irc {

class Server {
	private:
		std::string name;				// Name of encompassing object
		std::string hostname;
		std::string nick;
		std::string username;
		std::string realname;
		std::string password;
		bool log;
		bool connect;

		std::map<std::string, Channel> channels;	// List of channels we can join

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
		std::list<std::string> getChannels() ;
		bool isChannel(std::string channel) const;

	protected:

};

}
}
#endif
