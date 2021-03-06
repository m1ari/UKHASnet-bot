#include "message.h"
#include <iostream>

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
	void Message::setType(std::string in){
		type=in;
	}
	void Message::reply(std::string s, bool privmsg) const {
		if (isPRIVMSG()){
			if (server->isChannel(dest)){
				if (privmsg){
					server->sendMsg(dest,s);
				} else {
					server->sendNotice(dest,s);
				}
			} else {
				if (privmsg){
					server->sendMsg(nick,s);
				} else {
					server->sendNotice(nick,s);
				}
			}
		} else {
			// Wasn't a PRIVMSG, we're not supposed to reply
			std::cerr << "Message, trying to respond to something that's not a PRIVMSG" << std::endl << "\t" << s << std::endl;
		}
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
	bool Message::isPRIVMSG() const{
		if (type == "PRIVMSG")
			return true;
		return false;
	}
	bool Message::isNOTICE() const{
		if (type == "NOTICE")
			return true;
		return false;
	}

}
}
