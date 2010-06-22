#include "soap_request.h"

#include "emobiix_rpc_H.h"
#include "emobiix.nsmap"

using namespace std;

namespace emobiix
{

namespace soap_request
{

bool GetAuthentication(const std::string& URL, const std::string& deviceId, const std::string& userName, const std::string& password) 
{ 
	struct soap *s = soap_new();

	bool isAuthenticated = false;
	int ret = soap_call_ns__AuthenticationRequest(s, URL.c_str(), NULL, deviceId, userName, password, isAuthenticated);

 	soap_end(s);
	soap_free(s); 

	return isAuthenticated;
}

int GetBlockDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, std::string& mime, vector<pair<size_t, unsigned char *> >& blockData)
{
	struct soap *s = soap_new();

	vector<char *> blocks;

	ns__Timestamp ts(1, 5);

	xsd__base64Binary raw;
	int ret = soap_call_ns__BlockDataObjectRequest(s, URL.c_str(), NULL, deviceId, dataObjectURI, ts, raw);

	const int BLOCK = 2 * 1024;
	size_t rawSize = raw.getSize();
	unsigned char *rawPtr = raw.getPtr();

	unsigned char *chunk = NULL;
	while (rawSize > 0)
	{
		size_t chunkSize = BLOCK;
		if (rawSize < BLOCK)
			chunkSize = rawSize;

		chunk = (unsigned char *)malloc(chunkSize);
		memcpy(chunk, rawPtr, chunkSize);
		blockData.push_back(make_pair(chunkSize, chunk));

		rawSize -= chunkSize;
		rawPtr += chunkSize;
	}

	mime = raw.getMime();

 	soap_end(s);
	soap_free(s); 

	return ret == SOAP_OK;
}

int GetRecordDataObject(const std::string& uri, int id, std::vector<char *>& recordData)
{
	return 1;
}

int GetTextDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, std::string& textData)
{
	struct soap *s = soap_new();

	ns__Timestamp ts(1, 5);
	int ret = soap_call_ns__TextDataObjectRequest(s, URL.c_str(), NULL, deviceId, dataObjectURI, ts, textData);

 	soap_end(s);
	soap_free(s); 

	return ret == SOAP_OK;
}

int GetTreeDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, std::string& treeData)
{
	struct soap *s = soap_new();

	ns__Timestamp ts(1, 5);
	int ret = soap_call_ns__TreeDataObjectRequest(s, URL.c_str(), NULL, deviceId, dataObjectURI, ts, treeData);

 	soap_end(s);
	soap_free(s); 

	return ret == SOAP_OK;
}

int Test_Push(const std::string& data)
{
	struct soap *s = soap_new();

	bool isDelivered = false;
	int ret = soap_call_ns__DataObjectPushRequest(s, "http://0.0.0.0:23456", NULL, "1", data, isDelivered);

	soap_end(s);
	soap_free(s);

  return ret == SOAP_OK;
}

}

}
