#include <string>
#include <jansson.h>
#include <ctime>
#include "../database.h"
#include "nodes.h"


namespace UKHASnet {
namespace db {

	Nodes::Nodes(){
	}

	Nodes::~Nodes(){
	}
	

	int Nodes::getNodeID(std::string node){
		// Get the nodes ID looking up in the DB if needed
	}

	std::string Nodes::getNodeLastPacket(int nodeid){
		// Get the last packet from the DB
	}

	std::string Nodes::getNodeLastPacket(std::string node){
		int id=getNodeID(node);
		return getNodeLastPacket(id);
	}




}
}
