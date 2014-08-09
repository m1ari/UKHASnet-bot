#ifndef database_h_
#define database_h_

// #define __db_handler		// Code for the threaded Handler for notify events

//#include <queue>
//#include <map>
#include <pqxx/pqxx>
//#include <pthread.h>
//#include "irc/connection.h"
#include "../irc/message.h"
//#include "logger.h"

namespace UKHASnet {

class Database {
	private:
		static pqxx::connection dbh;
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
		bool getNode(irc::Message src, std::string text);

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
