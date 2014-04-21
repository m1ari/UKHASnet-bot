#include "server.h"

namespace UKHASnet {
namespace irc {

	Server::Server(){
	}

	Server::~Server(){
	}

	void Server::setName(std::string n){
		name=n;
	}

	void Server::setServer(std::string s){
		hostname=s;
	}

	void Server::setNick(std::string n){
		nick=n;
	}

	void Server::setUser(std::string u){
		username=u;
	}

	void Server::setRealname(std::string n){
		realname=n;
	}

	void Server::setPass(std::string p){
		password=p;
	}

	std::string Server::getName() const {
		return name;
	}
	std::string Server::getServer() const {
		return hostname;
	}
	std::string Server::getNick() const {
		return nick;
	}
	std::string Server::getUser() const {
		return username;
	}
	std::string Server::getRealname() const {
		return realname;
	}
	std::string Server::getPass() const {
		return password;
	}
}
}
