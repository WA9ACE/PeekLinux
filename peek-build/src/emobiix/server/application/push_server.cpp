#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

#include "emobiix_rpc_H.h"

#include "push_server.h"
#include "logger.h"

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

int ns__DataObjectPushRequest(struct soap*, int id, char *token, struct ns__DataObjectPushRequestResponse *param)
{
  return SOAP_OK;
}

int ns__AuthenticationRequest(struct soap*, char *devId, char *user, char *pass, bool &isAuthenticated) { return 403; } 
int ns__BlockDataObjectRequest(struct soap*, char *id, ns__Timestamp timestamp, xsd__base64Binary &blockData) { return 403; } 
int ns__TreeDataObjectRequest(struct soap*, char *id, ns__Timestamp timestamp, char *&m__treeData) { return 403; } 
int ns__RecordDataObjectRequest(struct soap*, int id, ns__Timestamp timestamp, recordArray &recordData) { return 403; } 
int ns__TextDataObjectRequest(struct soap*, int id, ns__Timestamp timestamp, char *&textData) { return 403; } 
