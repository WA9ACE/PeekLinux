#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <boost/bind.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "connection.h"
#include "logger.h"
#include "soap_request.h"
#include "shared_appdata.h"
#include "dataobject_factory.h"

#define DEVICE_UDP_LISTEN_PORT  "7"

using namespace std;

namespace emobiix {

using namespace boost::asio;

connection::connection(io_service& io_service)
	: strand_(io_service),
	socket_(io_service)
{
}

connection::~connection()
{
	shared_appdata::instance().remove("IP ADDRESS");
	TRACELOG("Terminating connection");
}

ip::tcp::socket& connection::socket()
{
	return socket_;
}

void connection::start()
{
	// detect connection breaks without any data traffic
	socket_base::keep_alive option(true);
	socket_.set_option(option);

	const string ip = socket_.remote_endpoint().address().to_string();
	DEBUGLOG("Received connection from: " << ip);

	INFOLOG("Requesting authentication from client");
	request_auth(reply_);

	async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));

	INFOLOG("Waiting for authentication response from client");
	socket_.async_read_some(buffer(buffer_), strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
}

void connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
{
	if (e)
	{
		ERRORLOG("A system error has occured: " << e << ": " << e.message());
		// If an error occurs then no new asynchronous operations are started. This
		// means that all shared_ptr references to the connection object will
		// disappear and the object will be destroyed automatically after this
		// handler returns. The connection class's destructor closes the socket.
		return;
	}

	TRACELOG("Handling a read of " << bytes_transferred);

	boost::tribool result = request_parser_.parse(request_, buffer_.data(), bytes_transferred);
	if (!result)
	{
		ERRORLOG("Received an invalid request");

		reply_ = reply::stock_error_reply();
		async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
	}
	else 
	{
		bool readRemains = false;
		if (result)
			readRemains = true;

		if (request_.packets.size())
		{
			TRACELOG("Received valid requests: " << request_.packets.size());

			handle_request(request_, reply_);
			if (reply_.packets.size())
				async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
		}

		TRACELOG("Resuming read...");
		socket_.async_read_some(buffer(buffer_), strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
	}
}

void connection::handle_write(const boost::system::error_code& e, std::size_t bytes_transferred)
{
	if (e)
	{
		ERRORLOG("A system error has occured: " << e << ": " << e.message());

		// Initiate graceful connection closure.
		boost::system::error_code ignored_ec;
		socket_.shutdown(ip::tcp::socket::shutdown_both, ignored_ec);
	}

	TRACELOG("Successfully wrote " << bytes_transferred);
	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}

void connection::request_auth(reply& rep)
{
	FRIPacketP *packet = new FRIPacketP;
	packet->packetTypeP.present = packetTypeP_PR_authRequestP;

	AuthRequestP_t &authReq = packet->packetTypeP.choice.authRequestP;
	authReq.authSaltP.buf = NULL;
	OCTET_STRING_fromBuf(&authReq.authSaltP, "NaCl", -1);

	authReq.authTypesP.list.array = NULL;
	authReq.authTypesP.list.size = 0;
	authReq.authTypesP.list.count = 0;
	authReq.authTypesP.list.free = NULL;

	AuthTypeP_t *authType = new AuthTypeP_t;
	*authType = AuthTypeP_atUsernamePasswordP;
	asn_sequence_add(&authReq.authTypesP.list, authType);

	rep.packets.push_back(packet);
}

void connection::handle_request(request& req, reply& rep)
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

void connection::handle_packet(FRIPacketP *packet, reply& rep)
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

void connection::handle_protocolHandshake(FRIPacketP*, reply& rep)
{

}

void connection::handle_authRequest(FRIPacketP*, reply& rep)
{
}

void connection::handle_authUserPass(FRIPacketP* packet, reply& rep)
{
	DEBUGLOG("Received authentication request");

	AuthUserPassP_t &userPass = packet->packetTypeP.choice.authUserPassP;
	string user((char *)userPass.authUsernameP.buf, userPass.authUsernameP.size);
	string pass((char *)userPass.authPasswordP.buf, userPass.authPasswordP.size);

	INFOLOG("User: " << user << ", pass: " << pass);

	map<string, string> extraFields;
	for (size_t i = 0; i < userPass.authExtrasP.list.count; ++i)
	{
		AuthExtraP_t &extra = *(userPass.authExtrasP.list.array[i]);
		string field((char *)extra.authExtraNameP.buf, extra.authExtraNameP.size);
		string value((char *)extra.authExtraValueP.buf, extra.authExtraValueP.size);

		extraFields[field] = value;
		INFOLOG("Extra field " << field << " = " << value);
	}

	FRIPacketP *authResponse = new FRIPacketP;
	authResponse->packetTypeP.present = packetTypeP_PR_authResponseP;

	map<string, string>::const_iterator IMEI = extraFields.find("IMEI");
	if (IMEI == extraFields.end())
	{
		ERRORLOG("Required field IMEI missing from authentication");
		authResponse->packetTypeP.choice.authResponseP = RequestResponseP_responseErrorP;
	}
	else if (soap_request::GetAuthentication("http://linux.emobiix.com:8082/cgi-bin/test.cgi", IMEI->second.c_str(), user.c_str(), pass.c_str()))
	{
		INFOLOG("Authentication successful");

		authResponse->packetTypeP.choice.authResponseP = RequestResponseP_responseOKP;
		appdata data = { this };
		shared_appdata::instance().put(IMEI->second, data);
	}
	else
	{
		ERRORLOG("Authentication failure");
		authResponse->packetTypeP.choice.authResponseP = RequestResponseP_responseFailP;
	}

	rep.packets.push_back(authResponse);
}

void connection::handle_authResponse(FRIPacketP*, reply& rep)
{

}

void connection::handle_subscriptionRequest(FRIPacketP*, reply& rep)
{

}

void connection::handle_subscriptionResponse(FRIPacketP*, reply& rep)
{

}

void connection::handle_dataObjectSyncStart(FRIPacketP* packet, reply& rep)
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

void connection::handle_dataObjectSync(FRIPacketP*, reply& rep)
{

}

void connection::handle_dataObjectSyncFinish(FRIPacketP* packet, reply& rep)
{
	DataObjectSyncFinishP &s = packet->packetTypeP.choice.dataObjectSyncFinishP;
	DEBUGLOG("Client Sync finished, seqId: " << s.syncSequenceIDP << ", response: " << s.responseP);

	start_serverSync(rep);
}

void connection::start_serverSync(reply& rep)
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
	if (!soap_request::GetTreeDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", url_request_, treeData))
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

bool connection::parseTree(DOMNode *node, vector<FRIPacketP *>& packets, int& nodeCount)
{
	if (!node)
		return false;

	int self = nodeCount;
	if (FRIPacketP *dataObject = dataobject_factory::create(node))
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
				dataobject_factory::addChild(packets.back());

			parseTree(node, packets, nodeCount);
		}

		node = node->getNextSibling();
		if (node && node->getNodeType() == DOMNode::ELEMENT_NODE) 
			dataobject_factory::goToTree(packets.back(), self);
	}
}

bool connection::parse(const char *doc, vector<FRIPacketP *>& packets)
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

void connection::push(const std::string &data)
{
	DEBUGLOG("Generating push request to device " << socket_.remote_endpoint().address().to_string() << " port " << DEVICE_UDP_LISTEN_PORT << " with data: " << data);

	using namespace boost::asio::ip;

  boost::asio::io_service io_service;

  udp::resolver resolver(io_service);
  udp::resolver::query query(udp::v4(), socket_.remote_endpoint().address().to_string(), DEVICE_UDP_LISTEN_PORT);
  udp::endpoint receiver_endpoint = *resolver.resolve(query);

  udp::socket device(io_service);
  device.open(udp::v4());
	device.send_to(boost::asio::buffer(data), receiver_endpoint);
}

}
