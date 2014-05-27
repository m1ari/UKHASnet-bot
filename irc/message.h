#ifndef irc_message_h_
#define irc_message_h_

#include <string>
#include "connection.h"

namespace UKHASnet {
namespace irc {

class Message {
	private:
		std::string nick;
		std::string user;
		std::string host;
		std::string dest;
		std::string text;
		irc::Connection *server;

	public:
		Message();
		~Message();

		bool hasServer() const;
		void setServer(irc::Connection *server);
		irc::Connection *getServer() const;	// Currently only used in handler - might be good to remove

		void setNick(std::string n);
		void setUser(std::string u);
		void setHost(std::string h);
		void setDest(std::string d);
		void setText(std::string m);
		void reply(std::string s, bool privmsg=false) const;	// false: NOTICE, true: PRIVMSG

		std::string getNick() const;
		std::string getUser() const;
		std::string getHost() const;
		std::string getDest() const;
		std::string getText() const;
	protected:
};


}
}


#endif
