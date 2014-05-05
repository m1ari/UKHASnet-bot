#include "server.h"

namespace UKHASnet {
namespace irc {

	Server::Server(){
		log=false;
		connect=true;
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
	void Server::addChannel(std::string c){
		channels.push_back(c);
	}
	void Server::delChannel(std::string c){
		for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++){
			if (c == *it){
				channels.erase(it);
			}
		}
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
	
	size_t Server::getNumChannels() const {
		return channels.size();
	}

	std::string Server::getChannel(int n) const {
		return channels.at(n);
	}
}
}
