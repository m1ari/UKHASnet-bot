#include <string>
#include <jansson.h>
#include <ctime>
#include "member.h"


namespace UKHASnet {
namespace irc {

	Member::Member(){
	}

/*
	Member::Member(json_t json){
		// Don't set inChannel from json
	}
*/

	Member::~Member(){
	}
	
	void Member::setNick(std::string in){
		nick=in;
	}
	void Member::setUser(std::string in){
		user=in;
	}
	void Member::setHost(std::string in){
		host=in;
	}
	void Member::setPresent(bool in){
		inChannel=in;
	}
	void Member::updateSeen(time_t in){
		if (in > 0){
			lastseen=in;
		} else {
			in=time(NULL);
		}
	}
	void Member::setSeenMesg(std::string m, time_t seen){
		seenmesg=m;
		updateSeen(seen);
	}

	std::string Member::getNick() const{
		return nick;
	}
	std::string Member::getUser() const{
		return user;
	}
	std::string Member::getHost() const{
		return host;
	}
	bool Member::isPresent() const{
		return inChannel;
	}
	time_t Member::getSeen() const{
		return lastseen;
	}
	std::string Member::getSeenMesg() const{
		char buff[100];
		time_t now=time(NULL);
		time_t gap=now-lastseen;

		if (gap > (7 * 24 * 60 * 60)){		// 7 Days
			struct tm *t;
			t=gmtime(&lastseen);
			strftime(buff,100, " on %c", t);
		} else if (gap > (2 * 24 * 60 * 60)) {	// 2 Day
			sprintf(buff, " %ld days ago", gap/(1*24*60*60));
		} else if (gap > (1 * 24 * 60 * 60)) {	// 1 Day
			sprintf(buff, " %ld day ago", gap/(1*24*60*60));
		} else if (gap > (2 * 60 * 60)){	// 2 Hour
			sprintf(buff, " %ld hours ago", gap/(1*60*60));
		} else if (gap > (1 * 60 * 60)){	// 1 Hour
			sprintf(buff, " %ld hour ago", gap/(1*60*60));
		} else {
			sprintf(buff, " very recently");
		}

		std::string m;
		m=nick + "(" + user + "@" + host + ")";
		if (inChannel){
			m+=" is in here and last spoke";
		} else {
			m+=" was last seen";
			if (seenmesg.size()>0){
				// expect seen message to say something like "changing nick, leaving"
				m+=" " + seenmesg;
			}
		}
		m+=buff;
		return m;
	}
/*
	json_t Member::dumpJson() const {
		// create json object
		// Add nick: (string) nick;
		// Add user: (string) user;
		// Add host: (string) host;
		// Add mesg: (string) mesg;
		// Add seen: (int) lastseen;
		// Add joined: (bool) inChannel;
		// return object;

	}
*/
}
}
