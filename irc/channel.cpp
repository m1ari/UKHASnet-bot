#include "channel.h"

namespace UKHASnet {
namespace irc {

	Channel::Channel(){
		// Need to look for state
		// file: <base>/state/server/channel
		// load json and call loadJson for members (should be construtor)
	}

	Channel::~Channel(){
		// need to dump state
		// itterate over members and call dumpJson()
	}

	void Channel::setName(std::string n){
		channel=n;
	}

	void Channel::setJoined(bool s){
		joined=s;
	}

	std::string Channel::getName() const{
		return channel;
	}

	bool Channel::isJoined() const{
		return joined;
	}

	void Channel::addMember(Member m){
		if (members.find(m.getNick()) == members.end()){ // Not Found
			members[m.getNick()] = m;
		} else {
			// Already Exists ...
			// Check User & Host match otherwise update
		}
	}

	void Channel::updateNickSeen(std::string n, time_t s){
		if (members.find(n) != members.end()){
			members[n].updateSeen(s);
		}
	}

/*
	void Channel::delMember(std::string c){
	}
*/
	
}
}
