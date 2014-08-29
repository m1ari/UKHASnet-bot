#include <iostream>
#include "server.h"
#include "channel.h"

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
	void Server::setLog(bool l){
		log=l;
	}
	void Server::setConnect(bool c){
		connect=c;
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
	bool Server::getLog() const {
		return log;
	}
	bool Server::getConnect() const {
		return connect;
	}

	void Server::addChannel(std::string c){
		// TODO Test if channel exists
		channels[c]=Channel(c);
	}

	void Server::delChannel(std::string c){
		std::cout << "TODO Server::delChannel(" << c << ")" << std::endl;
		// TODO Remove channel from list (or mark as non joinable)
	}
	size_t Server::getNumChannels() const {
		return channels.size();
	}
	std::list<std::string> Server::getChannels() {
		std::list<std::string> chans;
		for (std::map<std::string, Channel>::iterator it=channels.begin(); it!=channels.end(); ++it) {
			std::cout << "Adding: " << it->first << " to channel list" << std::endl;
			chans.push_back(it->first);
		}
		return chans;
	}
	bool Server::isChannel(std::string s) const {
		if (channels.find(s) != channels.end()){
			return true;
		} else {
			return false;
		}
	}
}
}
