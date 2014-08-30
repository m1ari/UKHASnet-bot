#ifndef database_h_
#define database_h_


#include <pqxx/pqxx>
#include <string>
#include "../irc/message.h"

namespace UKHASnet {

namespace db{
	class Nodes;
}

class Database {
	// TODO Should we use inheritance instead
	friend class db::Nodes;
	private:
		static pqxx::connection *dbh;
		static bool connected;

		std::string dbhost;
		std::string dbuser;
		std::string dbpass;
		std::string database;

	public:
		Database();
		Database(std::string h, std::string u, std::string p, std::string d);
		~Database();

		void setDBHost(std::string h);
		void setDBUser(std::string u);
		void setDBPass(std::string p);
		void setDatabase(std::string d);
		bool connect();
		bool isConnected() const;

		bool sendMessage(irc::Message msg);
		std::string getUpload(irc::Message msg);

	protected:

};

}
#endif


/*
	private:
		pthread_t threadid;
		static bool run;
		//static bool isConnected;

		// TODO Connection or Channel
		// Static map of items
		std::string id;
                std::map<std::string, std::string> dests; // string made up as servername + channel

		//irc::Connection *server;

		//std::map<std::string, Logger> logs; // string made up as servername + channel

		// Thread functions
                static void* entryPoint(void *pthis);
		void mainLoop();
	public:
		Database(std::string d, void *server);		// Used by a server process
		// Functions called by main
		void start();
		void stop();
*/
