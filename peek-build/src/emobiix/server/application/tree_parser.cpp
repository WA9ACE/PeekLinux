#include <string>

#include "logger.h"
#include "tree_parser.h"
#include "soap_request.h"
#include "dataobject_factory.h"

using namespace std;

namespace emobiix
{

tree_parser::tree_parser(const char* doc, const std::string& app_path, const std::string& connection_token)
: xml_parser(doc), 
	m_appPath(app_path),
	m_connectionToken(connection_token)
{
}

FRIPacketP* tree_parser::create(DOMNode *node)
{
  string nodeName = xml_parser::XMLToString(node->getNodeName());
  if (nodeName == "application")
    return createApplication(node);
  else if (nodeName == "view")
    return createView(node);
  else if (nodeName == "box")
    return createBox(node);
  else if (nodeName == "button")
    return createButton(node);
  else if (nodeName == "label")
    return createLabel(node);
  else if (nodeName == "entry")
    return createEntry(node);
  else if (nodeName == "image")
    return createImage(node);

  return NULL;
}

bool tree_parser::parseTree(DOMNode *node, std::vector<FRIPacketP *>& packets, int& nodeCount)
{
	if (!node)
		return false;

	int self = nodeCount;
	if (FRIPacketP *dataObject = create(node))
	{
		packets.push_back(dataObject);
		nodeCount++;
	}

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

FRIPacketP* tree_parser::createApplication(DOMNode *node)
{
	FRIPacketP *application = dataobject_factory::blockSyncListP();
	setCommonAttributes(application, node);
	dataobject_factory::addStringAttribute(application, "description", xml_parser::GetAttribute(node, "description").c_str());
	dataobject_factory::addStringAttribute(application, "icon", xml_parser::GetAttribute(node, "icon").c_str());
	dataobject_factory::addStringAttribute(application, "startupview", xml_parser::GetAttribute(node, "startupview").c_str());
	return application;
}

FRIPacketP* tree_parser::createView(DOMNode *node)
{
	FRIPacketP *view = dataobject_factory::blockSyncListP();
	setCommonAttributes(view, node);
	return view;
}

FRIPacketP* tree_parser::createBox(DOMNode *node)
{
	FRIPacketP *box = dataobject_factory::blockSyncListP();
	setCommonAttributes(box, node);
	dataobject_factory::addStringAttribute(box, "alignment", xml_parser::GetAttribute(node, "alignment").c_str());
	dataobject_factory::addStringAttribute(box, "packing", xml_parser::GetAttribute(node, "packing").c_str());
	dataobject_factory::addStringAttribute(box, "width", xml_parser::GetAttribute(node, "width").c_str());
	dataobject_factory::addStringAttribute(box, "height", xml_parser::GetAttribute(node, "height").c_str());
	dataobject_factory::addStringAttribute(box, "canfocus", xml_parser::GetAttribute(node, "canfocus").c_str());
	return box;
}

FRIPacketP* tree_parser::createButton(DOMNode *node)
{
	FRIPacketP *button = dataobject_factory::blockSyncListP();
	setCommonAttributes(button, node);
	dataobject_factory::addStringAttribute(button, "alignment", xml_parser::GetAttribute(node, "alignment").c_str());
	dataobject_factory::addStringAttribute(button, "packing", xml_parser::GetAttribute(node, "packing").c_str());
	dataobject_factory::addStringAttribute(button, "width", xml_parser::GetAttribute(node, "width").c_str());
	dataobject_factory::addStringAttribute(button, "height", xml_parser::GetAttribute(node, "height").c_str());
	dataobject_factory::addStringAttribute(button, "canfocus", xml_parser::GetAttribute(node, "canfocus").c_str());

	std::string prop;
	prop = xml_parser::GetAttribute(node, "accesskey");
	if (prop != "")
		dataobject_factory::addStringAttribute(button, "accesskey", prop.c_str());

	return button;
}

FRIPacketP* tree_parser::createLabel(DOMNode *node)
{
	FRIPacketP *label = dataobject_factory::blockSyncListP();
	setCommonAttributes(label, node);
	dataobject_factory::addStringAttribute(label, "alignment", xml_parser::GetAttribute(node, "alignment").c_str());
	dataobject_factory::addStringAttribute(label, "data", xml_parser::XMLToString(node->getFirstChild()->getNodeValue()).c_str());
	return label;
}

FRIPacketP* tree_parser::createEntry(DOMNode *node)
{
	FRIPacketP *entry = dataobject_factory::blockSyncListP();
	setCommonAttributes(entry, node);
	dataobject_factory::addStringAttribute(entry, "data", xml_parser::XMLToString(node->getFirstChild()->getNodeValue()).c_str());
	return entry;
}

FRIPacketP* tree_parser::createImage(DOMNode *node)
{
	FRIPacketP *image = dataobject_factory::blockSyncListP();
	setCommonAttributes(image, node);

	std::string src = xml_parser::GetAttribute(node, "src").c_str();

	std::string mime;
	vector<pair<size_t, unsigned char *> > blocks;
	soap_request::GetBlockDataObject(m_appPath, m_connectionToken, src, mime, blocks);

	dataobject_factory::addStringAttribute(image, "mime-type", mime.c_str());
	dataobject_factory::addDataAttribute(image, "src", blocks);
	return image;
}

void tree_parser::setCommonAttributes(FRIPacketP *packet, DOMNode *node)
{
	std::string prop;

  prop = xml_parser::XMLToString(node->getNodeName());
	dataobject_factory::addStringAttribute(packet, "type", prop.c_str());

	if ((prop = xml_parser::GetAttribute(node, "id")) != "")
		dataobject_factory::addStringAttribute(packet, "id", prop.c_str());

	if ((prop = xml_parser::GetAttribute(node, "name")) != "")
	  dataobject_factory::addStringAttribute(packet, "name", prop.c_str());

	if ((prop = xml_parser::GetAttribute(node, "script")) != "")
	  dataobject_factory::addStringAttribute(packet, "script", prop.c_str());

	if ((prop = xml_parser::GetAttribute(node, "onreturn")) != "")
	  dataobject_factory::addStringAttribute(packet, "onreturn", prop.c_str());
}

}

