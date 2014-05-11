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


	// The following functions should change to use the Channel object at a later date
	void Server::addChannel(std::string c){
		channels.insert(c);
	}

	void Server::delChannel(std::string c){
		channels.erase(c);
	}
	size_t Server::getNumChannels() const {
		return channels.size();
	}
/*
	Channel Server::getChannel(int n) const {
		return channels.at(n);
	}
*/
	bool Server::isChannel(std::string s) const {
		if (channels.find(s) != channels.end()){
			return true;
		} else {
			return false;
		}
	}
}
}
