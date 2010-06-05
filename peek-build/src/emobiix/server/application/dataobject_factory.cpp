#include "dataobject_factory.h"
#include "FRIPacketP.h"
#include "xml.h"
#include "logger.h"
#include "soap_request.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>

using namespace std;

namespace emobiix
{

FRIPacketP *dataobject_factory::create(DOMNode *node)
{
	string nodeName = XML::XMLToString(node->getNodeName());
	if (nodeName == "emobiix-application") 
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

void dataobject_factory::addStringAttribute(FRIPacketP *packet, const char *attribute, const char *value)
{
	TRACELOG("Adding " << attribute << " = " << value);

	SyncOperandP_t *syncOp = new SyncOperandP_t;
	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, attribute);
	syncOp->syncP.present = syncP_PR_syncSetP;
	syncOp->syncP.choice.syncSetP.buf = NULL;
	OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncSetP, value, strlen(value) + 1);
	asn_sequence_add(&packet->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, syncOp);
}

void dataobject_factory::addDataAttribute(FRIPacketP *packet, const char *attribute, vector<pair<size_t, unsigned char *> >& data)
{
	TRACELOG("Adding " << attribute << " = " << data.size());

	size_t offset = 0;
	for (size_t i = 0; i < data.size(); ++i)
	{
		SyncOperandP_t *syncOp = new SyncOperandP_t;
		syncOp->fieldNameP.buf = NULL;
		OCTET_STRING_fromString(&syncOp->fieldNameP, attribute);

		syncOp->syncP.present = syncP_PR_syncModifyP;
		syncOp->syncP.choice.syncModifyP.modifyDataP.buf = NULL;
		OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncModifyP.modifyDataP, (char *)data[i].second, data[i].first);
		syncOp->syncP.choice.syncModifyP.modifySizeP = data[i].first;
		syncOp->syncP.choice.syncModifyP.modifyOffsetP = offset;
		asn_sequence_add(&packet->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, syncOp);

		TRACELOG("Adding data chunk " << i << " of size " << data[i].first << " at offset " << offset);

		offset += data[i].first;
	}
}

FRIPacketP* dataobject_factory::createDataObject(const char *szType, DOMNode *node)
{
	FRIPacketP *object = new FRIPacketP;
	object->packetTypeP.present = packetTypeP_PR_dataObjectSyncP;
	DataObjectSyncP &s = object->packetTypeP.choice.dataObjectSyncP;
	s.syncSequenceIDP = 1;

	s.syncListP.present = SyncListP_PR_blockSyncListP;
	s.syncListP.choice.blockSyncListP.list.array = NULL;
	s.syncListP.choice.blockSyncListP.list.size = 0;
	s.syncListP.choice.blockSyncListP.list.count = 0;

	addStringAttribute(object, "type", szType);
	return object;
}

void dataobject_factory::setCommonAttributes(FRIPacketP *packet, DOMNode *node)
{
	std::string prop;
	if ((prop = XML::GetAttribute(node, "id")) != "")
		addStringAttribute(packet, "id", prop.c_str());

	if ((prop = XML::GetAttribute(node, "name")) != "")
	  addStringAttribute(packet, "name", prop.c_str());

	if ((prop = XML::GetAttribute(node, "script")) != "")
	  addStringAttribute(packet, "script", prop.c_str());

	if ((prop = XML::GetAttribute(node, "onreturn")) != "")
	  addStringAttribute(packet, "onreturn", prop.c_str());
}

FRIPacketP* dataobject_factory::createApplication(DOMNode *node)
{
	FRIPacketP *application = createDataObject("application", node);
	addStringAttribute(application, "name", XML::GetAttribute(node, "name").c_str());
	addStringAttribute(application, "description", XML::GetAttribute(node, "description").c_str());
	addStringAttribute(application, "icon", XML::GetAttribute(node, "icon").c_str());
	addStringAttribute(application, "startupview", XML::GetAttribute(node, "startupview").c_str());
	return application;
}

FRIPacketP* dataobject_factory::createView(DOMNode *node)
{
	FRIPacketP *view = createDataObject("view", node);
	addStringAttribute(view, "name", XML::GetAttribute(node, "name").c_str());
	return view;
}

FRIPacketP* dataobject_factory::createBox(DOMNode *node)
{
	FRIPacketP *box = createDataObject("box", node);
	setCommonAttributes(box, node);
	addStringAttribute(box, "alignment", XML::GetAttribute(node, "alignment").c_str());
	addStringAttribute(box, "packing", XML::GetAttribute(node, "packing").c_str());
	addStringAttribute(box, "width", XML::GetAttribute(node, "width").c_str());
	addStringAttribute(box, "height", XML::GetAttribute(node, "height").c_str());
	addStringAttribute(box, "canfocus", XML::GetAttribute(node, "canfocus").c_str());
	return box;
}

FRIPacketP* dataobject_factory::createButton(DOMNode *node)
{
	FRIPacketP *button = createDataObject("button", node);
	setCommonAttributes(button, node);
	addStringAttribute(button, "alignment", XML::GetAttribute(node, "alignment").c_str());
	addStringAttribute(button, "packing", XML::GetAttribute(node, "packing").c_str());
	addStringAttribute(button, "width", XML::GetAttribute(node, "width").c_str());
	addStringAttribute(button, "height", XML::GetAttribute(node, "height").c_str());
	addStringAttribute(button, "canfocus", XML::GetAttribute(node, "canfocus").c_str());

	std::string prop;
	prop = XML::GetAttribute(node, "accesskey");
	if (prop != "")
		addStringAttribute(button, "accesskey", prop.c_str());

	return button;
}

FRIPacketP* dataobject_factory::createLabel(DOMNode *node)
{
	FRIPacketP *label = createDataObject("label", node);
	setCommonAttributes(label, node);
	addStringAttribute(label, "type", XML::GetAttribute(node, "type").c_str());
	addStringAttribute(label, "alignment", XML::GetAttribute(node, "alignment").c_str());
	addStringAttribute(label, "data", XML::XMLToString(node->getFirstChild()->getNodeValue()).c_str());
	return label;
}

FRIPacketP* dataobject_factory::createEntry(DOMNode *node)
{
	FRIPacketP *entry = createDataObject("entry", node);
	setCommonAttributes(entry, node);
	addStringAttribute(entry, "data", XML::XMLToString(node->getFirstChild()->getNodeValue()).c_str());
	return entry;
}

FRIPacketP* dataobject_factory::createImage(DOMNode *node)
{
	FRIPacketP *image = createDataObject("image", node);
	setCommonAttributes(image, node);
  addStringAttribute(image, "onreturn", XML::GetAttribute(node, "onreturn").c_str());

	std::string src = XML::GetAttribute(node, "src").c_str();

	std::string mime;
	vector<pair<size_t, unsigned char *> > blocks;
	soap_request::GetBlockDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", src, mime, blocks);

	addStringAttribute(image, "mime-type", mime.c_str());
	addDataAttribute(image, "src", blocks);
	return image;
}

void dataobject_factory::addChild(FRIPacketP* packet)
{
	TRACELOG("Moving to a child node");

	SyncOperandP_t *syncOp = new SyncOperandP_t;
	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, "");
	syncOp->syncP.present = syncP_PR_nodeOperationP;
	syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeAddP;
	syncOp->syncP.choice.nodeOperationP.choice.nodeAddP = nodeAddP_nodeChildP;
	asn_sequence_add(&packet->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, syncOp);
}

void dataobject_factory::goToTree(FRIPacketP* packet, int index)
{
	TRACELOG("Moving to parents node: " << index);

	SyncOperandP_t *syncOp = new SyncOperandP_t;
	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, "");
	syncOp->syncP.present = syncP_PR_nodeOperationP;
	syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeGotoTreeP;
	syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP = index;
	asn_sequence_add(&packet->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, syncOp);
}


}

