#ifndef __EMOBIIX_TREE_PARSER_H__
#define __EMOBIIX_TREE_PARSER_H__

#include <vector>
#include <string>

#include "xml_parser.h"

struct FRIPacketP;

namespace emobiix
{

class tree_parser : public xml_parser
{
public:
	tree_parser(const char* doc, const std::string& app_path, const std::string& connection_token, int syncId);
	virtual ~tree_parser() { }
	bool parse(std::vector<FRIPacketP *>& packets);

private:
	bool create(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool parseTree(DOMNode *node, std::vector<FRIPacketP *>& packets, int& nodeCount);

	bool createApplication(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool createView(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool createBox(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool createButton(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool createLabel(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool createEntry(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool createImage(DOMNode *node, std::vector<FRIPacketP *>& packets);
	bool createArray(DOMNode *node, std::vector<FRIPacketP *>& packets);
	
	void setCommonAttributes(FRIPacketP *packet, DOMNode *node);

	std::string m_appPath;
	std::string m_connectionToken;
	int m_currentSyncId;
};

}

#endif
