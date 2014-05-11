#include "message.h"

namespace UKHASnet {
namespace irc {

	Message::Message(){
	}

	Message::~Message(){
	}

	bool Message::hasServer() const {
		if (server==NULL){
			return false;
		} else {
			return true;
		}
	}
	void Message::setServer(irc::Connection *s){
		server=(irc::Connection *)s;
	}

	irc::Connection* Message::getServer() const {
		return server;
	}

	void Message::setNick(std::string in){
		nick=in;
	}
	void Message::setUser(std::string in){
		user=in;
	}
	void Message::setHost(std::string in){
		host=in;
	}
	void Message::setDest(std::string in){
		dest=in;
	}
	void Message::setText(std::string in){
		text=in;
	}

	std::string Message::getNick() const{
		return nick;
	}
	std::string Message::getUser() const{
		return user;
	}
	std::string Message::getHost() const{
		return host;
	}
	std::string Message::getDest() const{
		return dest;
	}
	std::string Message::getText() const{
		return text;
	}

}
}
