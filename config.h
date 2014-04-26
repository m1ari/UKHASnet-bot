#ifndef config_h_
#define config_h_

#include <jansson.h>
#include <string>
#include <list>
#include "irc/server.h"

namespace UKHASnet {

//class irc::Server;	// Forward declare has issues

class Config {
	private:
		json_t *json;
		std::string filename;

	public:
		Config();
		~Config();
		void setFile(std::string file);
		void loadConfig();
		void saveConfig() const;

		// IRC Servers
		std::list<std::string> getIRCServerList() const;
		irc::Server getIRCServer(std::string server) const;
		//void addIRCServer(irc::Server server);
		//void delIRCServer(std::string server);

		std::list<std::string> getIRCChannels(std::string server) const;
		//void addIRCChannel(std::string server, std::string channel);
		//void delIRCChannel(std::string server, std::string channel);

		std::string getString(int count, ... ) const;
		std::string getArray(int count, ... ) const;

	protected:

};

}
#endif
