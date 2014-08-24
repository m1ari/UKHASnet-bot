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
			//pqxx::result res=txn.exec("select id from ukhasnet.nodes where name='" + s_node + "';");
			pqxx::result res=txn.exec("select id from ukhasnet.nodes where name=" + txn.quote(s_node) );
			txn.commit();

			if (res.size() ==1 ){
				//int id=res[0][0].as<int>();
				int id=res[0]["id"].as<int>();
				std::cout << "Nodes::getNodeID got " << id << " for " << s_node << std::endl;
				node2id[s_node]=id;
				return id;
			} else {
				std::cout << "Bad result size in Nodes::getNodeID " << res.size() << std::endl;
			}
		}
	}

	std::string Nodes::getNodeLastPacket(int nodeid){
	/* 

	ukhasnet-prod1=# select lastpacket from ukhasnet.nodes where id=63;
 	lastpacket
	------------
	919673

	ukhasnet-prod1=# select * from ukhasnet.packet where id=919673;
	id   | originid | sequence | checksum
	--------+----------+----------+----------
 	919673 |       63 | t        |    54574


	ukhasnet-prod1=# select * from ukhasnet.packet_rx where packetid= 919673;
	id    | packetid | gatewayid |      packet_rx_time       | uploadid
	---------+----------+-----------+---------------------------+----------
 	1155696 |   919673 |         6 | 2014-08-24 12:14:25.99302 |  1157400


		// select * from ukhasnet.packet left join ukhasnet.nodes on nodes.lastpacket=packet.id where nodes.id=63;
	*/

	/*	pqxx::work w(dbh, "mytransaction");
		w.exec("INSERT INTO city(city_name) VALUES ('Ljubljana');");
		w.commit();
*/

	}

	std::string Nodes::getNodeLastPacket(std::string node){
		int id=getNodeID(node);
		return getNodeLastPacket(id);
	}




}
}
