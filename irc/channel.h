#ifndef irc_channel_h_
#define irc_channel_h_

#include <string>
//#include <map>
//#include "member.h"

namespace UKHASnet {
namespace irc {

class Channel {
	private:
		std::string channel;		// Channel Name
		bool joinable;			// Should we join this channel
		bool joined;			// Have we joined this channel

		// Does the Channel need to know about it's server?
		//Connection *server;
		//std::map<std::string, Member> members;	// Unordered map appears in C++11 and might be faster
		// void delMember(std::string c); // Only delete on cleanup cycle ?
	public:
		Channel();
		Channel(std::string name);
		~Channel();

		void setName(std::string name);
		void setJoinable(bool state);
		void setJoined(bool state);

		std::string getName() const;
		bool isJoinable() const;
		bool isJoined() const;

		//void setServer(Connection *s);
		//void memberJoin(std:string usrstring);
		//void memberPart(std::string usrstring, );
		//void memberNick(std::string usrstring, nick);
		//void addMember(Member m);
		//void updateNickSeen(std::string n, time_t s=0);
		//Member getMember(std::string name) const;
	protected:

};
}
}
#endif
