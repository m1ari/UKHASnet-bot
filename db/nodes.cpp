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
		// TODO Check node is safe
		std::string s_node = node;

		if (node2id.count(s_node) >0){
			return node2id[s_node];
		} else {
			Database db;
			pqxx::work txn(*db.dbh, "getid");
			pqxx::result res=txn.exec("select id from ukhasnet.nodes where name=" + txn.quote(s_node) );
			// TODO use txn.esc(node) instead ?
			txn.commit();

			if (res.size() ==1 ){
				int id=res[0]["id"].as<int>();
				std::cout << "Nodes::getNodeID got " << id << " for " << s_node << std::endl;
				node2id[s_node]=id;
				return id;
			} else {
				std::cout << "Bad result size in Nodes::getNodeID " << res.size() << std::endl;
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
		pqxx::result node=txn.exec("select lastpacket,name from ukhasnet.nodes where id=" + txn.quote(nodeid));
		if (node.size() != 1 ){
			return "Unable to get Node info";
		}
		nodename=node[0]["name"].c_str();
		lastpacket=node[0]["lastpacket"].as<int>();

		// Get Pakcet
		pqxx::result packet=txn.exec("select * from ukhasnet.packet where id=" + txn.quote(lastpacket));
		if (packet.size() != 1 ){
			return "Unable to find Packet";
		}

		/* 
		ukhasnet-prod1=# select * from ukhasnet.packet where id=919673;
		id   | originid | sequence | checksum
		--------+----------+----------+----------
 		919673 |       63 | t        |    54574
		*/

		pqxx::result packetrx=txn.exec("select packet_rx_time, uploadid, name from ukhasnet.packet_rx left join ukhasnet.nodes on packet_rx.gatewayid=nodes.id where packetid=" + txn.quote(lastpacket));

		/*
		ukhasnet-prod1=# select * from ukhasnet.packet_rx where packetid= 919673;
		id    | packetid | gatewayid |      packet_rx_time       | uploadid
		---------+----------+-----------+---------------------------+----------
 		1155696 |   919673 |         6 | 2014-08-24 12:14:25.99302 |  1157400
		ukhasnet-prod1=# select * from ukhasnet.packet_rx left join ukhasnet.nodes on packet_rx.gatewayid=nodes.id  where packetid= 919673;
   		id    | packetid | gatewayid |      packet_rx_time       | uploadid | id | name |         owner         | locationid | typeid | lastpacket
		---------+----------+-----------+---------------------------+----------+----+------+-----------------------+------------+--------+------------
 		1155696 |   919673 |         6 | 2014-08-24 12:14:25.99302 |  1157400 |  6 | AB   | jcoxon - Gateway Node |     310090 |      2 |     921745

		*/
		txn.commit();

		std::string reply;
		reply = "Node " + nodename;
		for (pqxx::result::const_iterator row = packetrx.begin(); row != packetrx.end(); ++row)	{
			// TODO add in path
			reply += ", Uploaded by " + row["name"].as<std::string>() + " at " + row["packet_rx_time"].as<std::string>() + " id is " + row["uploadid"].as<std::string>();
		}
		return reply;
	}

	std::string Nodes::getNodeLastPacket(std::string node){
		int id=getNodeID(node);
		return getNodeLastPacket(id);
	}




}
}
