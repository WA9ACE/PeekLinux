#include <string>

#include "logger.h"
#include "record_parser.h"
#include "soap_request.h"
#include "dataobject_factory.h"

using namespace std;

namespace emobiix
{

record_parser::record_parser(const char* doc, const std::string& app_path, const std::string& connection_token)
: xml_parser(doc), 
	m_appPath(app_path),
	m_connectionToken(connection_token)
{
}

bool record_parser::parseTree(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	if (!node)
		return false;

	node = node->getFirstChild();

	int index = 0;
	while (node) 
	{
		if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMNamedNodeMap *attr = node->getAttributes();
			if (attr) {
				for (size_t i = 0; i < attr->getLength(); ++i)
				{
					string attribute = xml_parser::XMLToString(attr->item(i)->getNodeName());
					string value = xml_parser::XMLToString(attr->item(i)->getNodeValue());
					if (attribute == "idminor") {
						index = atoi(value.c_str());
						break;
					}
				}

				addElement(node, packets.back(), ++index);
			}
		}

		node = node->getNextSibling();
	}
}

bool record_parser::parse(std::vector<FRIPacketP *>& packets)
{
	DOMDocument *document = m_parser->getDocument();
	if (!document)
	{
		ERRORLOG("Failed to parse document");
		return false;
	}

	parseTree(document->getFirstChild(), packets);
	return true;
}

void record_parser::addElement(DOMNode *node, FRIPacketP* packet, int index)
{
	TRACELOG("Adding element: " << index << "...");

	DOMNamedNodeMap *attr = node->getAttributes();
	if (!attr)
		return;

	string prop = xml_parser::XMLToString(node->getNodeName());

	for (size_t i = 0; i < attr->getLength(); ++i)
	{
		string attribute = xml_parser::XMLToString(attr->item(i)->getNodeName());
		string value = xml_parser::XMLToString(attr->item(i)->getNodeValue());
		if (attribute == "minorid")
			index = atoi(value.c_str());
	}
	
	RecordSyncListP_t *record =	dataobject_factory::recordSyncP(index, 0,
			prop == "delete" ? 1 : 0);
	for (size_t i = 0; i < attr->getLength(); ++i)
	{
		string attribute = xml_parser::XMLToString(attr->item(i)->getNodeName());
		string value = xml_parser::XMLToString(attr->item(i)->getNodeValue());

		TRACELOG("Adding record field [" << attribute << "] = [" << value << "]");

		SyncOperandP_t *syncOp = dataobject_factory::syncOperandP(attribute.c_str());
		syncOp->syncP.present = syncP_PR_syncSetP;
		syncOp->syncP.choice.syncSetP.buf = NULL;
		OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncSetP, value.c_str(), strlen(value.c_str()) + 1);

		asn_sequence_add(&record->recordFieldListP->list, syncOp);
	}

	DataObjectSyncP &s = packet->packetTypeP.choice.dataObjectSyncP;
	asn_sequence_add(&s.syncListP.choice.recordSyncListP.list, record);
}

}

