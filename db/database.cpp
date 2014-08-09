//#include <queue>
#include <pthread.h>
//#include <cstdio>
#include <iostream>
#include <pqxx/pqxx>
//#include <unistd.h>
#include "database.h"
//#include "irc/server.h"
#include "irc/message.h"

namespace UKHASnet {
	// Static vars
	//pqxx::connection Database::dbh = NULL;
	bool Database::connected=false;

	Database::Database(){
	}

	Database::Database(std::string h, std::string u, std::string p, std::string d){
		setDBHost(h);
		setDBUser(u);
		setDBPass(p);
		setDatabase(d);
		connect();
	}

	Database::~Database(){
	}

	void Database::setDBHost(std::string h){
		if (connected==false){
			dbhost=h;
		}
	}
	void Database::setDBUser(std::string u){
		if (connected==false){
			dbuser=u;
		}
	}
	void Database::setDBPass(std::string p){
		if (connected==false){
			dbpass=p;
		}
	}
	void Database::setDatabase(std::string d){
		if (connected==false){
			database=d;
		}
	}

	bool Database::connect(){
		std::string dbh_connect;
		if (!dbhost.empty()){
			if (!dbh_connect.empty())
				dbh_connect+=" ";
			dbh_connect+="host=" + dbhost;
		}
		if (!dbuser.empty()){
			if (!dbh_connect.empty())
				dbh_connect+=" ";
			dbh_connect+="user=" + dbuser;
		}
		if (!dbpass.empty()){
			if (!dbh_connect.empty())
				dbh_connect+=" ";
			dbh_connect+="password=" + dbpass;
		}
		if (!database.empty()){
			if (!dbh_connect.empty())
				dbh_connect+=" ";
			dbh_connect+="dbname=" + database;
		}
		//port=5432

		// TODO Need to add some tests around this 
		pqxx::connection dbh(dbh_connect);

		connected=true;

		return connected;
	}

	bool Database::isConnected() const {
		return connected;
	}


	bool Database::sendMessage(irc::Message msg){
		/* DB method for !msg IRC commands
		 * !msg node@gateway Message
		 */ 

		//std::cout << "Message From: " << msg.getNick() << " in Channel " << msg.getDest() << " Reads: " << msg.getText() << std::endl;

		std::string data=msg.getText();
		size_t p;
		std::string node;
		std::string gateway;

		// Check the string starts with !msg and strip it out
		if (data.find("!msg ") == 0){
			data.erase(0,5);
		} else {
			std::cout << "Error: Database was given a bad string to parse" << std::endl;
		}

		// Get node
		p=data.find("@");
		if (p != std::string::npos){
			node=data.substr(0,p);
			data.erase(0,p+1);
		} else {
			std::cout << "Error: Database(sendMessage) Can't determine node to send to" << std::endl;
		}

		// get gateway
		p=data.find(" ");
		if (p != std::string::npos){
			gateway=data.substr(0,p);
			data.erase(0,p+1);
		} else {
			std::cout << "Error: Database(sendMessage) Can't determine gateway to send to" << std::endl;
		}

		std::cout << "Processing: " << "Node:" << node << " Gateway: " << gateway << " Data: " << data << std::endl;
		/*
		pqxx::work w(dbh, "msg");  
		w.exec("INSERT INTO ukhasnet.messages (from) VALUES ('Ljubljana');");  
		w.commit(); 
		
		INSERT INTO ukhasnet.irc_msg values (timestamp, nick, chan, node, gateway, message) values ();
		*/
		return false;
	}

	bool Database::getNode(irc::Message src, std::string text){
		/*
		pqxx::work w(dbh, "mytransaction");  
		w.exec("INSERT INTO city(city_name) VALUES ('Ljubljana');");  
		w.commit(); 
		*/
		return false;
	}

}
/*
	bool Database::run=false;
	//bool Database::isConnected=false;
	Database::Database(std::string d, void *s){
		//id=d;
		//dests[id]=s;
		// Store in a Map ?
		//server = (irc::Connection*)s;
	}

	void* Database::entryPoint(void *pthis){
		Database *self= (Database*)pthis;
		self->mainLoop();
		pthread_exit(0);
	}

	void Database::mainLoop(){
		printf("Database process started\n");
		irc::Message msg;
		while (run){
			// Connect to the notify socket
			// Wait for notify message
			// Process message
		}
		printf("Database prcess ending\n");
	}

	void Database::start(){
		//if (server!=NULL){
			//fprintf(stderr, "Error: Trying to start handler thread with server defined\n");
			//return;
			// Only start the thread if this instance doesn't have a server ID set
		//}

		if (run==true){
			fprintf(stderr, "Error: Database thread already running\n");
			return;
		}

		run=true;
		pthread_create(&threadid,NULL,&Database::entryPoint,this);
	}

	void Database::stop(){
		run=false;
		pthread_join(threadid,NULL);
	}
*/
