#ifndef irc_member_h_
#define irc_member_h_

#include <string>

namespace UKHASnet {
namespace irc {

class Member {
	private:
		std::string nick;
		std::string user;
		std::string host;
		std::string seenmesg;
		time_t lastseen;
		bool inChannel;

	public:
		Member();
		// Member(json_t json);
		~Member();

		void setNick(std::string nick);
		void setUser(std::string user);
		void setHost(std::string host);
		void setPresent(bool present);
		void updateSeen(time_t seen=0);
		void setSeenMesg(std::string message, time_t seen=0);

		std::string getNick() const;
		std::string getUser() const;
		std::string getHost() const;
		bool isPresent() const;
		time_t getSeen() const;
		std::string getSeenMesg() const;
		//json_t dumpJson() const;

	protected:

};

}
}
#endif
