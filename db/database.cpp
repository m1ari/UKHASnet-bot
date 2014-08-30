//#include <queue>
#include <pthread.h>
//#include <cstdio>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <exception>
//#include <unistd.h>
#include "database.h"
#include "nodes.h"
//#include "irc/server.h"
#include "../irc/message.h"

// Some good bits in https://github.com/radiowarwick/digiplay_legacy/blob/master/src/DbTrigger.cpp

namespace UKHASnet {
	// Static vars
	pqxx::connection *Database::dbh = NULL;
	bool Database::connected=false;

	Database::Database(){
		// TODO We need to track instances
	}

	Database::Database(std::string h, std::string u, std::string p, std::string d){
		setDBHost(h);
		setDBUser(u);
		setDBPass(p);
		setDatabase(d);
		connect();
	}

	Database::~Database(){
		// TODO when instances =0 
		// delete dbh;

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
		dbh = new pqxx::connection(dbh_connect);

		connected=true;

		// Create prepared statements
		dbh->prepare("getUpload", "select time, packet, rssi, name  from ukhasnet.upload left join ukhasnet.nodes on upload.nodeid=nodes.id where upload.id=$1;")("integer");
		dbh->prepare("irc_msg", "INSERT INTO ukhasnet.irc_msg (src_nick, src_chan, gatewayid, nodeid, message, status) values ($1, $2, $3, $4, $5, $6);")("varchar", pqxx::prepare::treat_string)("varchar", pqxx::prepare::treat_string)("integer")("integer")("varchar", pqxx::prepare::treat_string)("varchar", pqxx::prepare::treat_string);

		// Prepared statements for db::Nodes Class
		dbh->prepare("nodeName2id","select id from ukhasnet.nodes where name=$1;")("varchar", pqxx::prepare::treat_string);
		dbh->prepare("nodeID2Name","select lastpacket,name from ukhasnet.nodes where id=$1;")("integer");
		dbh->prepare("getPacket","select originid, sequence, checksum from ukhasnet.packet where id=$1;")("integer");
		dbh->prepare("getPacketRX","select packet_rx.id as rxid, packet_rx_time, uploadid, name from ukhasnet.packet_rx left join ukhasnet.nodes on packet_rx.gatewayid=nodes.id where packetid=$1;")("integer");
		dbh->prepare("getPacketPath","select position, name from ukhasnet.path left join ukhasnet.nodes on path.nodeid=nodes.id where packet_rx_id=$1 order by position;")("integer");

		return connected;
	}

	bool Database::isConnected() const {
		return connected;
	}


	bool Database::sendMessage(irc::Message msg){
		/* DB method for !msg IRC commands
		 * !msg node@gateway Message
		 */ 

		std::string data=msg.getText();
		size_t p;
		std::string node;
		std::string gateway;

		// TODO we should possibly change to a regex in future

		// Check the string starts with !msg and strip it out
		if (data.find("!msg ") == 0){
			data.erase(0,5);
		} else {
			std::cout << "Error: Database was given a bad string to parse" << std::endl;
		}

		// Trim from any NL/CR (they should only appear at end of line
		p = data.find_first_of("\r\n");
		data.erase(p);

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

		db::Nodes n;
		int nodeid=n.getNodeID(node);
		int gatewayid=n.getNodeID(gateway);

		if ((nodeid > 0) && (gatewayid > 0)) {
			try {
				pqxx::work txn(*dbh, "msg");
				txn.prepared("irc_msg")(msg.getNick())(msg.getDest())(gatewayid)(nodeid)(data)("Pending").exec();
				txn.commit();
				return true;
			} catch (const std::exception &e) {
				std::cerr << e.what() << std::endl;
				//TODO Error Message
				return false;
			}
		} else {
			// Error Message
			return false;
		}
	}

	std::string Database::getUpload(irc::Message msg){
		std::string data=msg.getText();

		// Check the string starts with !upload and strip it out
		if (data.find("!upload ") == 0){
			data.erase(0,8);
		} else {
			std::cout << "Error: Database was given a bad string to parse" << std::endl;
			return "Unable to parse " + data;
		}

		// Trim from any NL/CR (they should only appear at end of line
		size_t p = data.find_first_of("\r\n");
		data.erase(p);

		// Run the query and build up a reply if successful
		std::string out;
		try {
			pqxx::work txn(*dbh, "getupload");
			pqxx::result upload = txn.prepared("getUpload")(data).exec();
			txn.commit();

			if (upload.size() != 1){
				out = "Unable to find uploaded packet with ID=" + data;
			} else {
				out  = "Packet: " + upload[0]["packet"].as<std::string>();
				out += " was recieved by " + upload[0]["name"].as<std::string>();
				out += " at " + upload[0]["time"].as<std::string>();
				out += " with rssi of " + upload[0]["rssi"].as<std::string>();
			}
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			out = "Unable to find uploaded packet with ID=" + data;
		}
		return out;
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
