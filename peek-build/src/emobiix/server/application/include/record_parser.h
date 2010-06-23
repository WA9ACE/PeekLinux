#ifndef __EMOBIIX_RECORD_PARSER_H__
#define __EMOBIIX_RECORD_PARSER_H__

#include <vector>
#include <string>

#include "xml_parser.h"

struct FRIPacketP;

namespace emobiix
{

class record_parser : public xml_parser
{
public:
	record_parser(const char* doc, const std::string& app_path, const std::string& connection_token);
	virtual ~record_parser() { }
	bool parse(std::vector<FRIPacketP *>& packets);

private:
	bool parseTree(DOMNode *node, std::vector<FRIPacketP *>& packets);
	
	void addElement(DOMNode* node, FRIPacketP* packet, int index);

	std::string m_appPath;
	std::string m_connectionToken;
};

}

#endif
