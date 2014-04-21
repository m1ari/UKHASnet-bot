#ifndef config_h_
#define config_h_

#include <jansson.h>
#include <string>

namespace UKHASnet {

class Config {
	private:
		json_t *json;
		std::string filename;

	public:
		Config();
		~Config();
		void setFile(std::string file);
		void loadConfig();

		size_t getNumIrcServers() const;
		void getIrcServer(int server) const;


		std::string getString(int count, ... ) const;
		std::string getArray(int count, ... ) const;
		void saveConfig() const;

	protected:

};

}
#endif
