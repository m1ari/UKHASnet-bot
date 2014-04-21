#ifndef irc_server_h_
#define irc_server_h_

#include <string>

namespace UKHASnet {
namespace irc {

class Server {
	private:
		std::string name;
		std::string hostname;
		std::string nick;
		std::string username;
		std::string realname;
		std::string password;

	public:
		Server();
		~Server();

		void setName(std::string name);
		void setServer(std::string hostname);
		void setNick(std::string nick);
		void setUser(std::string username);
		void setRealname(std::string name);
		void setPass(std::string password);

		std::string getName() const;
		std::string getServer() const;
		std::string getNick() const;
		std::string getUser() const;
		std::string getRealname() const;
		std::string getPass() const;

	protected:

};

}
}
#endif
