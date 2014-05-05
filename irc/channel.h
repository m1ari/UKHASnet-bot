#ifndef irc_channel_h_
#define irc_channel_h_

#include <string>
#include <vector>
#include <map>
#include "member.h"

namespace UKHASnet {
namespace irc {

class Channel {
	private:
		std::string channel;
		bool joined;


		// Does the Channel need to know about it's server?
		//Connection *server;
		std::map<std::string, Member> members;	// Unordered map appears in C++11 and might be faster

		// void delMember(std::string c); // Only delete on cleanup cycle ?
	public:
		Channel();
		~Channel();

		//void setServer(Connection *s);

		void setName(std::string name);
		void setJoined(bool state);

		std::string getName() const;
		bool isJoined() const;

		void addMember(Member m);
		void updateNickSeen(std::string n, time_t s=0);

	protected:

};

}
}
#endif
