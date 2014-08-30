#include <string>
//#include <jansson.h>
//#include <ctime>
#include <iostream>
#include <pqxx/pqxx>
#include "database.h"
#include "nodes.h"


namespace UKHASnet {
namespace db {
	// Declare statics
	std::map <std::string, int> Nodes::node2id;

	Nodes::Nodes(){
	}

	Nodes::~Nodes(){
	}
	
	int Nodes::getNodeID(std::string node){
		std::string s_node = node;

		if (node2id.count(s_node) >0){
			return node2id[s_node];
		} else {
			Database db;
			pqxx::work txn(*db.dbh, "getid");
			pqxx::result res=txn.prepared("nodeName2id")(s_node).exec();
			txn.commit();

			if (res.size() ==1 ){
				int id=res[0]["id"].as<int>();
				std::cout << "Nodes::getNodeID got " << id << " for " << s_node << std::endl;
				node2id[s_node]=id;
				return id;
			} else {
				std::cout << "Bad result size in Nodes::getNodeID looking for " << s_node << " Got " << res.size() << std::endl;
			}
		}
		return 0;
	}

	std::string Nodes::getNodeLastPacket(int nodeid){
		std::string nodename;
		int lastpacket;

		Database db;
		pqxx::work txn(*db.dbh, "getnode");

		// Get the Nodes name and last packet ID
		pqxx::result node=txn.prepared("nodeID2Name")(nodeid).exec();
		if (node.size() != 1 ){
			return "Unable to get Node info";
		}

		nodename=node[0]["name"].c_str();
		if (node[0]["lastpacket"].is_null()){
			return "Unable to find most recent packet for " + nodename;
			// TODO We could query for the most recent packet the hard way in this case.
		} else {
			lastpacket=node[0]["lastpacket"].as<int>();
		}

		// Get Pakcet - We don't currently use the data but it's a test that we have a valid packet
		pqxx::result packet=txn.prepared("getPacket")(lastpacket).exec();
		if (packet.size() != 1 ){
			return "Unable to find Packet";
		}

		// Get the received packets
		pqxx::result packetrx=txn.prepared("getPacketRX")(lastpacket).exec();

		// Generate the reply
		std::string reply;
		reply = "Node " + nodename;
		for (pqxx::result::const_iterator row = packetrx.begin(); row != packetrx.end(); ++row)	{
			reply += ", Uploaded by " + row["name"].as<std::string>();
			pqxx::result path=txn.prepared("getPacketPath")(row["rxid"].as<int>()).exec();
			if (path.size() >= 1 ){
				reply += " via [";
				for (pqxx::result::const_iterator rxrow = path.begin(); rxrow != path.end(); ++rxrow) {
					if (rxrow != path.begin()) {
						reply +=",";
					}
					reply +=rxrow["name"].as<std::string>();
				}
				reply += "]";
			}
			reply += " at " + row["packet_rx_time"].as<std::string>() + " id is " + row["uploadid"].as<std::string>();
		}
		txn.commit();
		return reply;
	}

	std::string Nodes::getNodeLastPacket(std::string node){
		int id=getNodeID(node);
		return getNodeLastPacket(id);
	}
}
}
