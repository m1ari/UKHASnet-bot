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

		std::list<std::string> getIRCServerList() const;
		irc::Server getIrcServer(std::string server) const;
		std::string getString(int count, ... ) const;
		std::string getArray(int count, ... ) const;
		void saveConfig() const;

	protected:

};

}
#endif
