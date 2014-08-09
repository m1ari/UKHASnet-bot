#ifndef db_nodes_h_
#define db_nodes_h_

#include <string>
#include <map>

namespace UKHASnet {
namespace db {

class Nodes {
	private:
		static std::map <std::string, int> node2id;

	public:
		Nodes();
		~Nodes();


		int getNodeID(std::string node);			// Get the nodes ID looking up in the DB if needed
		std::string getNodeLastPacket(int nodeid);		// Get the last packet from the DB
		std::string getNodeLastPacket(std::string node);	// Wrapper for getNodeID and getNodeLastPacket(int)

	protected:

};

}
}
#endif
