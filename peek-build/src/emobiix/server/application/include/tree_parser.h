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
	tree_parser(const char* doc, const std::string& app_path, const std::string& connection_token);
	virtual ~tree_parser() { }
	bool parse(std::vector<FRIPacketP *>& packets);

private:
	FRIPacketP* create(DOMNode *node);
	bool parseTree(DOMNode *node, std::vector<FRIPacketP *>& packets, int& nodeCount);

	FRIPacketP* createApplication(DOMNode *node);
	FRIPacketP* createView(DOMNode *node);
	FRIPacketP* createBox(DOMNode *node);
	FRIPacketP* createButton(DOMNode *node);
	FRIPacketP* createLabel(DOMNode *node);
	FRIPacketP* createEntry(DOMNode *node);
	FRIPacketP* createImage(DOMNode *node);
	
	void setCommonAttributes(FRIPacketP *packet, DOMNode *node);

	std::string m_appPath;
	std::string m_connectionToken;
};

}

#endif
