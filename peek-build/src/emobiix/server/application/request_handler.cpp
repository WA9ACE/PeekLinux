#include "request_handler.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "reply.h"
#include "request.h"
#include "soap_request.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace std;

namespace emobiix {

static std::string XMLToString(const XMLCh *xszValue)
{
	if (xszValue == NULL)
		return "";

	char* szValue = XMLString::transcode(xszValue);

	std::string sValue;
	sValue = szValue;

	XMLString::release(&szValue);
	return sValue;
}

static std::string GetAttribute(DOMNode* pElem, const char* szAttr)
{
	XMLCh * szXmlAttr = XMLString::transcode(szAttr);
	const XMLCh* xszValue = ((DOMElement *)pElem)->getAttribute(szXmlAttr); 
	XMLString::release(&szXmlAttr);

	if(xszValue)
		return XMLToString(xszValue);

	return "";
}

request_handler::request_handler(const std::string& app_path)
	: app_path_(app_path)
{
}

void request_handler::handle_request(request& req, reply& rep)
{
	if (!req.packets.size())
	{
		ERRORLOG("Unknown packet received, will not handle");
		rep = reply::stock_error_reply();
		return;
	}

	for (size_t i = 0; i < req.packets.size(); ++i)
		handle_packet(req.packets[i], rep);

	req.packets.clear();
}

void request_handler::handle_packet(FRIPacketP *packet, reply& rep)
{
	DEBUGLOG("Received packet of type: " << packet->packetTypeP.present);
	switch (packet->packetTypeP.present)
	{
		case packetTypeP_PR_protocolHandshakeP:
			handle_protocolHandshake(packet, rep);
			break;

		case packetTypeP_PR_authRequestP:
			handle_authRequest(packet, rep);
			break;

		case packetTypeP_PR_authUserPassP:
			handle_authUserPass(packet, rep);
			break;

		case packetTypeP_PR_authResponseP:
			handle_authResponse(packet, rep);
			break;

		case packetTypeP_PR_subscriptionRequestP:
			handle_subscriptionRequest(packet, rep);
			break;

		case packetTypeP_PR_subscriptionResponseP:
			handle_subscriptionResponse(packet, rep);
			break;

		case packetTypeP_PR_dataObjectSyncStartP:
			handle_dataObjectSyncStart(packet, rep);
			break;

		case packetTypeP_PR_dataObjectSyncP:
			handle_dataObjectSync(packet, rep);
			break;

		case packetTypeP_PR_dataObjectSyncFinishP:
			handle_dataObjectSyncFinish(packet, rep);
			break;

		case packetTypeP_PR_NOTHING:
			break;
	}
}

void request_handler::handle_protocolHandshake(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_authRequest(FRIPacketP*, reply& rep)
{
	//	rep.packet->packetTypeP.present = packetTypeP_PR_authResponseP;
	//	rep.packet.packetType.choice.authResponse = RequestResponse_responseOK;
}

void request_handler::handle_authUserPass(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_authResponse(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_subscriptionRequest(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_subscriptionResponse(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_dataObjectSyncStart(FRIPacketP* packet, reply& rep)
{
	DataObjectSyncStartP &s = packet->packetTypeP.choice.dataObjectSyncStartP;
	DEBUGLOG("Client Sync started for " << s.urlP.buf << ", revision: " << s.dataObjectStampMinorP << "." << s.dataObjectStampMajorP << ", seqId: " << s.syncSequenceIDP);

	if (char *slash = strrchr((const char *)s.urlP.buf, '/'))
		url_request_ = slash + 1;
	else
		url_request_ = "sample";

	// clean up the packet!
	free(s.urlP.buf);
	free(packet);
}

void request_handler::handle_dataObjectSync(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_dataObjectSyncFinish(FRIPacketP* packet, reply& rep)
{
	DataObjectSyncFinishP &s = packet->packetTypeP.choice.dataObjectSyncFinishP;
	DEBUGLOG("Client Sync finished, seqId: " << s.syncSequenceIDP << ", response: " << s.responseP);

	start_serverSync(rep);
}

void request_handler::start_serverSync(reply& rep)
{
	DEBUGLOG("Starting server sync");

	FRIPacketP *start = new FRIPacketP;
	start->packetTypeP.present = packetTypeP_PR_dataObjectSyncP;
	DataObjectSyncP &s = start->packetTypeP.choice.dataObjectSyncP;

	s.syncListP.present = SyncListP_PR_blockSyncListP;
	s.syncListP.choice.blockSyncListP.list.array = NULL;
	s.syncListP.choice.blockSyncListP.list.size = 0;
	s.syncListP.choice.blockSyncListP.list.count = 0;
	s.syncSequenceIDP = 1;

	rep.packets.push_back(start);

	string treeData;
	if (!soap_request::get_treeDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", url_request_, treeData))
	{
		ERRORLOG("NO tree data...");
		return;
	}

	TRACELOG("Tree data received: " << treeData);
	parse(treeData.c_str(), rep.packets);

	DEBUGLOG("Finishing server sync");
	FRIPacketP *finish = new FRIPacketP;
	finish->packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
	finish->packetTypeP.choice.dataObjectSyncFinishP.responseP = RequestResponseP_responseExpiredP;
	finish->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP = 1;

	rep.packets.push_back(finish);
}

void addStringAttribute(FRIPacketP *packet, const char *attribute, const char *value)
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

void addDataAttribute(FRIPacketP *packet, const char *attribute, const char *value)
{
	SyncOperandP_t *syncOp = new SyncOperandP_t;
	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, attribute);

	syncOp->syncP.present = syncP_PR_syncModifyP;
	syncOp->syncP.choice.syncModifyP.modifyDataP.buf = NULL;
	OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncModifyP.modifyDataP, 0, 0); //field->field.data.bytes, field->field.data.size);
	syncOp->syncP.choice.syncModifyP.modifySizeP = 0; //field->field.data.size;
	syncOp->syncP.choice.syncModifyP.modifyOffsetP = 0;
	asn_sequence_add(&packet->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, syncOp);
}

void addChild(FRIPacketP* packet)
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

void goToTree(FRIPacketP* packet, int index)
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

FRIPacketP* createDataObject()
{
	FRIPacketP *object = new FRIPacketP;
	object->packetTypeP.present = packetTypeP_PR_dataObjectSyncP;
	DataObjectSyncP &s = object->packetTypeP.choice.dataObjectSyncP;
	s.syncSequenceIDP = 1;

	s.syncListP.present = SyncListP_PR_blockSyncListP;
	s.syncListP.choice.blockSyncListP.list.array = NULL;
	s.syncListP.choice.blockSyncListP.list.size = 0;
	s.syncListP.choice.blockSyncListP.list.count = 0;

	return object;
}

FRIPacketP* createBox(DOMNode *node, int index)
{
	FRIPacketP *box = createDataObject();

	char buf[32] = ""; sprintf(buf, "%d", index);
	addStringAttribute(box, "node", buf);
	addStringAttribute(box, "alignment", GetAttribute(node, "alignment").c_str());
	addStringAttribute(box, "packing", GetAttribute(node, "packing").c_str());
	addStringAttribute(box, "width", GetAttribute(node, "width").c_str());
	addStringAttribute(box, "height", GetAttribute(node, "height").c_str());
	addStringAttribute(box, "id", GetAttribute(node, "id").c_str());
	addStringAttribute(box, "canfocus", GetAttribute(node, "canfocus").c_str());
  addStringAttribute(box, "onreturn", GetAttribute(node, "onreturn").c_str());

	const std::string scriptProp = GetAttribute(node, "script").c_str();
	if (scriptProp != "")
	  addStringAttribute(box, "script", scriptProp.c_str());

	const std::string nameProp = GetAttribute(node, "name").c_str();
	if (nameProp != "")
	  addStringAttribute(box, "name", nameProp.c_str());

	return box;
}

FRIPacketP* createButton(DOMNode *node, int index)
{
	FRIPacketP *button = createDataObject();

	char buf[32] = ""; sprintf(buf, "%d", index);
	addStringAttribute(button, "node", buf);
	addStringAttribute(button, "alignment", GetAttribute(node, "alignment").c_str());
	addStringAttribute(button, "packing", GetAttribute(node, "packing").c_str());
	addStringAttribute(button, "width", GetAttribute(node, "width").c_str());
	addStringAttribute(button, "height", GetAttribute(node, "height").c_str());
	addStringAttribute(button, "id", GetAttribute(node, "id").c_str());
	addStringAttribute(button, "canfocus", GetAttribute(node, "canfocus").c_str());
	addStringAttribute(button, "onreturn", GetAttribute(node, "onreturn").c_str());

	const std::string scriptProp = GetAttribute(node, "script").c_str();
	if (scriptProp != "")
	  addStringAttribute(button, "script", scriptProp.c_str());

	const std::string nameProp = GetAttribute(node, "name").c_str();
	if (nameProp != "")
	  addStringAttribute(button, "name", nameProp.c_str());

	return button;
}

FRIPacketP* createLabel(DOMNode *node, int index)
{
	FRIPacketP *label = createDataObject();

	char buf[32] = ""; sprintf(buf, "%d", index);
	addStringAttribute(label, "node", buf);
	addStringAttribute(label, "id", GetAttribute(node, "id").c_str());
	addStringAttribute(label, "type", GetAttribute(node, "type").c_str());
	addStringAttribute(label, "alignment", GetAttribute(node, "alignment").c_str());
	addStringAttribute(label, "data", XMLToString(node->getFirstChild()->getNodeValue()).c_str());
  addStringAttribute(label, "onreturn", GetAttribute(node, "onreturn").c_str());

	const std::string scriptProp = GetAttribute(node, "script").c_str();
	if (scriptProp != "")
	  addStringAttribute(label, "script", scriptProp.c_str());

	const std::string nameProp = GetAttribute(node, "name").c_str();
	if (nameProp != "")
	  addStringAttribute(label, "name", nameProp.c_str());

	return label;
}

FRIPacketP* createEntry(DOMNode *node, int index)
{
	FRIPacketP *entry = createDataObject();

	char buf[32] = ""; sprintf(buf, "%d", index);
	addStringAttribute(entry, "node", buf);
	addStringAttribute(entry, "id", GetAttribute(node, "id").c_str());
	addStringAttribute(entry, "data", XMLToString(node->getFirstChild()->getNodeValue()).c_str());
  addStringAttribute(entry, "onreturn", GetAttribute(node, "onreturn").c_str());

	const std::string scriptProp = GetAttribute(node, "script").c_str();
	if (scriptProp != "")
	  addStringAttribute(entry, "script", scriptProp.c_str());

	const std::string nameProp = GetAttribute(node, "name").c_str();
	if (nameProp != "")
	  addStringAttribute(entry, "name", nameProp.c_str());

	return entry;
}

FRIPacketP* createImage(DOMNode *node, int index)
{
	FRIPacketP *image = createDataObject();

	char buf[32] = ""; sprintf(buf, "%d", index);
	addStringAttribute(image, "node", buf);
	addStringAttribute(image, "id", GetAttribute(node, "id").c_str());
	addStringAttribute(image, "src", GetAttribute(node, "src").c_str());
  addStringAttribute(image, "onreturn", GetAttribute(node, "onreturn").c_str());

	const std::string scriptProp = GetAttribute(node, "script").c_str();
	if (scriptProp != "")
	  addStringAttribute(image, "script", scriptProp.c_str());

	const std::string nameProp = GetAttribute(node, "name").c_str();
	if (nameProp != "")
	  addStringAttribute(image, "name", nameProp.c_str());

	return image;
}

static bool parseTree(DOMNode *node, vector<FRIPacketP *>& packets, int& nodeCount)
{
	if (!node)
		return false;

	int self = nodeCount;
	string nodeName = XMLToString(node->getNodeName());

	if (nodeName == "box") {
		packets.push_back(createBox(node, nodeCount));
		nodeCount++;
	}
	if (nodeName == "button") {
		packets.push_back(createButton(node, nodeCount));
		nodeCount++;
	}
	else if (nodeName == "label")
	{
		packets.push_back(createLabel(node, nodeCount));
		nodeCount++;
	}
	else if (nodeName == "entry")
	{
		packets.push_back(createEntry(node, nodeCount));
		nodeCount++;
	}
	else if (nodeName == "image")
	{
		packets.push_back(createImage(node, nodeCount));
		nodeCount++;
	}
	else
	{
		// TODO
	}


	node = node->getFirstChild();
	while (node) {
		nodeName = XMLToString(node->getNodeName());
		if (nodeName != "#text") {
			if (self != nodeCount)
				addChild(packets.back());
			parseTree(node, packets, nodeCount);
		}

		node = node->getNextSibling();
		if (node && nodeName != "#text") {
			goToTree(packets.back(), self);
		}
	}
}

bool request_handler::parse(const char *doc, vector<FRIPacketP *>& packets)
{
	XMLPlatformUtils::Initialize();
	{
		XercesDOMParser *parser = new XercesDOMParser;
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(false);
		parser->setDoSchema(false);
		parser->setLoadExternalDTD(false); // perhaps later?

		MemBufInputSource buffer((const XMLByte *)doc, strlen(doc), "MemoryBuffer");
		parser->parse(buffer);

		DOMDocument *document = parser->getDocument();

		int nodeCount = 0;
		parseTree(document->getFirstChild(), packets, nodeCount);

		delete parser;
	}
	XMLPlatformUtils::Terminate();
}

}
