#include <vector>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

#include "emobiix_rpc_H.h"

#include "push_server.h"
#include "logger.h"
#include "shared_appdata.h"
#include "connection.h"

namespace emobiix
{

using namespace boost::asio;

push_server::push_server(const std::string& host_name, const std::string& port)
{
	m_soap = new struct soap;
	soap_init(m_soap);

	int nPort = boost::lexical_cast<int>(port);
	int m_socket = soap_bind(m_soap, host_name.c_str(), nPort, 100);

	if (m_socket < 0)
	{
		ERRORLOG("Failed to bind on soap server socket");
		soap_print_fault(m_soap, stderr); 
	}
}

push_server::~push_server()
{
	delete m_soap;
}

void push_server::run()
{
	if (m_socket < 0)
		return;

	TRACELOG("Starting to process requests...");
	for (;;)
	{
		int client_socket = soap_accept(m_soap); 
		if (client_socket < 0)
		{
			ERRORLOG("Accept terminated");
			soap_print_fault(m_soap, stderr);
			break;
		}

		DEBUGLOG("Accepted connection (" << client_socket << ") from " << 
				((m_soap->ip >> 24) & 0xFF) << "." << 
				((m_soap->ip >> 16) & 0xFF) << "." << 
				((m_soap->ip >> 8) & 0xFF) << "." << 
				(m_soap->ip & 0xFF));

		if (soap_serve(m_soap) != SOAP_OK)
		{
			ERRORLOG("Invalid request received");
			soap_print_fault(m_soap, stderr); 
		}

		soap_destroy(m_soap);
		soap_end(m_soap);
	} 
}

void push_server::stop()
{
	// close master socket and detach context 
	soap_done(m_soap); 
}

}

int ns__DataObjectPushRequest(struct soap*, std::string deviceId, std::string dataObjectURI, bool &isDelivered)
{
	emobiix::appdata data;
	if (!emobiix::shared_appdata::instance().get(deviceId, data))
		return 404;

	data.device->push(dataObjectURI);
	return SOAP_OK;
}

int ns__AuthenticationRequest(struct soap*, std::string deviceId, std::string userName, std::string password, bool &isAuthenticated) { return 403; }
int ns__BlockDataObjectRequest(struct soap*, std::string deviceId, std::string dataObjectURI, ns__Timestamp timeStamp, xsd__base64Binary &binaryData) { return 403; }
int ns__TreeDataObjectRequest(struct soap*, std::string deviceId, std::string dataObjectURI, ns__Timestamp timeStamp, std::string &m__treeData) { return 403; }
int ns__RecordDataObjectRequest(struct soap*, std::string deviceId, std::string dataObjectURI, ns__Timestamp timestamp, recordArray &recordData) { return 403; }
int ns__TextDataObjectRequest(struct soap*, std::string deviceId, std::string dataObjectURI, ns__Timestamp timestamp, std::string &textData) { return 403; }
