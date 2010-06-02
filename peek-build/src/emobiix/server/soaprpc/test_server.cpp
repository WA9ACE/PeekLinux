#include <cstdio>

#include "stdsoap2.h"
#include "emobiix_rpc_Stub.h"
#include "emobiix_rpc_emobiixObject.h" // get server object
#include "emobiix.nsmap" // get namespace bindings

using namespace std;

int main()
{
   emobiixService c;
   return c.serve(); // calls soap_serve to serve as CGI application (using stdin/out)
}

int ns__AuthenticationRequest(struct soap* soap, xsd__string devId, xsd__string user, xsd__string password, bool &isAuthenticated)
{
	cerr << "Received authentication request for [" << user << "], [" << password << "]" << endl;

	if (!strcmp(user, "bob") && !strcmp(password, "torulethemall"))
		isAuthenticated = true;
	else
		isAuthenticated = false;

	cerr << "Authenticated? " << isAuthenticated << endl;

	return SOAP_OK;
}

int ns__BlockDataObjectRequest(struct soap* soap, xsd__string id, ns__Timestamp timestamp, xsd__base64Binary &rawData)
{
	FILE *fd = fopen(id, "rb");

	fseek(fd, 0L, SEEK_END);
	int filesize = ftell(fd);
	fseek(fd, 0L, SEEK_SET);

	char *type = "unknown";
	if (char *dot = strrchr(id, '.'))
		type = dot + 1;

	rawData = xsd__base64Binary(soap, filesize, type);

	fread(rawData.getPtr(), rawData.getSize(), 1, fd);
	fclose(fd);

	return SOAP_OK;
}

int ns__RecordDataObjectRequest(struct soap* soap, int id, ns__Timestamp timestamp, recordArray &recordData)
{
	recordData = recordArray(soap, 3);

	recordData[0] = "Record #1";
	recordData[1] = "Record #2";
	recordData[2] = "Record #3";
	
	return SOAP_OK;
}

int ns__TextDataObjectRequest(struct soap* soap, int id, ns__Timestamp timestamp, char*& textData)
{
	textData = "some textual data";
	return SOAP_OK;
}

int ns__TreeDataObjectRequest(struct soap* soap, xsd__string id, ns__Timestamp timestamp, XML& m__treeData)
{
	char path[2048] = "";
	sprintf(path, "%s.xml", id);

	FILE *fd = fopen(path, "rb");
	if (!fd)
	{
		m__treeData = "<emobiix-gui></emobiix-gui>";
		return SOAP_OK;
	}

	fseek(fd, 0L, SEEK_END);
	int filesize = ftell(fd);
	fseek(fd, 0L, SEEK_SET);

	m__treeData = (XML)soap_malloc(soap, filesize * sizeof(char));

	fread(m__treeData, filesize, 1, fd);
	m__treeData[filesize] = 0;

	fclose(fd);

	return SOAP_OK;
}

int ns__DataObjectPushRequest(struct soap*, int id, char *token, struct ns__DataObjectPushRequestResponse *param)
{
	return SOAP_OK;
}

