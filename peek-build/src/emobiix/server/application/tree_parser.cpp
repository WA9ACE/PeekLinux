#include <string>

#include "logger.h"
#include "tree_parser.h"
#include "soap_request.h"
#include "dataobject_factory.h"

using namespace std;

namespace emobiix
{

tree_parser::tree_parser(const char* doc, const std::string& app_path, const std::string& connection_token, int syncId)
: xml_parser(doc), 
	m_appPath(app_path),
	m_connectionToken(connection_token),
	m_currentSyncId(syncId)
{
}

bool tree_parser::create(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
  string nodeName = xml_parser::XMLToString(node->getNodeName());

  if (nodeName == "application")
    return createApplication(node, packets);
  else if (nodeName == "view")
    return createView(node, packets);
  else if (nodeName == "box")
    return createBox(node, packets);
  else if (nodeName == "button")
    return createButton(node, packets);
  else if (nodeName == "label" || nodeName == "text")
    return createLabel(node, packets);
  else if (nodeName == "entry")
    return createEntry(node, packets);
  else if (nodeName == "script")
    return createScript(node, packets);
  else if (nodeName == "image")
    return createImage(node, packets);
  else if (nodeName == "array")
    return createArray(node, packets);
	else 
		return createGeneric(node, packets);

	return false;
}

bool tree_parser::parseTree(DOMNode *node, std::vector<FRIPacketP *>& packets, int& nodeCount)
{
	if (!node)
		return false;

	int self = nodeCount;
	if (create(node, packets))
		nodeCount++;

	node = node->getFirstChild();
	while (node) 
	{
		if (node->getNodeType() == DOMNode::ELEMENT_NODE) 
		{
			if (self != nodeCount)
				asn_sequence_add(&(packets.back())->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, dataobject_factory::syncOperandP_nodeAddP());

			parseTree(node, packets, nodeCount);
		}

		node = node->getNextSibling();
		if (node && node->getNodeType() == DOMNode::ELEMENT_NODE) 
			asn_sequence_add(&(packets.back())->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, dataobject_factory::syncOperandP_nodeGotoTreeP(self));
	}
}

bool tree_parser::parse(std::vector<FRIPacketP *>& packets)
{
	DOMDocument *document = m_parser->getDocument();
	if (!document)
	{
		ERRORLOG("Failed to parse document");
		return false;
	}

	int nodeCount = 0;
	parseTree(document->getFirstChild(), packets, nodeCount);

	return true;
}

bool tree_parser::createGeneric(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	// TODO Explicitly list nodes
	FRIPacketP *generic = dataobject_factory::blockSyncListP(m_currentSyncId); 
	setCommonAttributes(generic, node);
	packets.push_back(generic);
	return true;
}

bool tree_parser::createApplication(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *application = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(application, node);
	packets.push_back(application);
	return true;
}

bool tree_parser::createView(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *view = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(view, node);
	packets.push_back(view);
	return true;
}

bool tree_parser::createBox(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *box = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(box, node);
	packets.push_back(box);
	return true;
}

bool tree_parser::createArray(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *array = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(array, node);
	packets.push_back(array);
	return true;
}

bool tree_parser::createButton(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *button = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(button, node);
	packets.push_back(button);
	return true;
}

bool tree_parser::createLabel(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *label = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(label, node);

	if (node->getFirstChild())
		dataobject_factory::addStringAttribute(label, "data", xml_parser::XMLToUTF8String(node->getFirstChild()->getNodeValue()).c_str());
	else
		dataobject_factory::addStringAttribute(label, "data", "");

	packets.push_back(label);
	return true;
}

bool tree_parser::createEntry(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *entry = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(entry, node);

	if (node->getFirstChild())
		dataobject_factory::addStringAttribute(entry, "data", xml_parser::XMLToUTF8String(node->getFirstChild()->getNodeValue()).c_str());
	else
		dataobject_factory::addStringAttribute(entry, "data", "");

	packets.push_back(entry);
	return true;
}

bool tree_parser::createScript(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	FRIPacketP *script = dataobject_factory::blockSyncListP(m_currentSyncId);
	setCommonAttributes(script, node);

	if (node->getFirstChild())
	{
		string scriptString("");
		DOMNode *child = node->getFirstChild();
		while (child)
		{
			scriptString += xml_parser::XMLToUTF8String(child->getNodeValue());
			child = child->getNextSibling();
		}
	
		dataobject_factory::addStringAttribute(script, "data", scriptString.c_str());
	}

	packets.push_back(script);
	return true;
}

bool tree_parser::createImage(DOMNode *node, std::vector<FRIPacketP *>& packets)
{
	std::string mime;
	vector<pair<size_t, unsigned char *> > blocks;
	map<string, string> params;
	soap_request::GetBlockDataObject(m_appPath, m_connectionToken, xml_parser::GetAttribute(node, "src"), params, mime, blocks);

	FRIPacketP *image = NULL;
	size_t offset = 0;
	for (size_t i = 0; i < blocks.size(); ++i)
	{
		image = dataobject_factory::blockSyncListP(m_currentSyncId);
		setCommonAttributes(image, node);
		dataobject_factory::addStringAttribute(image, "mime-type", mime.c_str());
		dataobject_factory::addDataAttribute(image, "data", blocks[i].second, blocks[i].first, offset);

		TRACELOG("Adding data chunk " << i << " of size " << blocks[i].first << " at offset " << offset);

		packets.push_back(image);
		offset += blocks[i].first;
	}

	return true;
}

void tree_parser::setCommonAttributes(FRIPacketP *packet, DOMNode *node)
{
	string prop = xml_parser::XMLToString(node->getNodeName());
	dataobject_factory::addStringAttribute(packet, "type", prop.c_str());

	DOMNamedNodeMap *attr = node->getAttributes();
	if (!attr)
		return;

	for (size_t i = 0; i < attr->getLength(); ++i)
	{
		string attribute = xml_parser::XMLToString(attr->item(i)->getNodeName());
		string value = xml_parser::XMLToString(attr->item(i)->getNodeValue());

		dataobject_factory::addStringAttribute(packet, attribute.c_str(), value.c_str());
	}
}

}

