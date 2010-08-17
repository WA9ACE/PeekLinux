#include "soap_request.h"

#include "emobiix_rpc_H.h"
#include "emobiix.nsmap"

using namespace std;

namespace emobiix
{

namespace soap_request
{

static void ParamMapToVector(const std::map<std::string, std::string>& paramMap, std::vector<ns__KeyValue>& paramVector)
{
	for (std::map<std::string, std::string>::const_iterator it = paramMap.begin(), end = paramMap.end(); it != end; ++it)
		paramVector.push_back(ns__KeyValue(it->first, it->second));
}

bool GetAuthentication(const std::string& URL, const std::string& deviceId, const std::string& userName, const std::string& password, const std::map<std::string, std::string>& params) 
{ 
	struct soap *s = soap_new();

	bool isAuthenticated = false;
	
	std::vector<ns__KeyValue> requestParams;
	ParamMapToVector(params, requestParams);

	int ret = soap_call_ns__AuthenticationRequest(s, URL.c_str(), NULL, deviceId, userName, password, &requestParams, isAuthenticated);

 	soap_end(s);
	soap_free(s); 

	return isAuthenticated;
}

int GetBlockDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& mime, vector<pair<size_t, unsigned char *> >& blockData)
{
	struct soap *s = soap_new();

	vector<char *> blocks;

	ns__Timestamp ts(1, 5);
	xsd__base64Binary raw;

	std::vector<ns__KeyValue> requestParams;
	ParamMapToVector(params, requestParams);

	int ret = soap_call_ns__BlockDataObjectRequest(s, URL.c_str(), NULL, deviceId, dataObjectURI, ts, &requestParams, raw);

	const int BLOCK = 3 * 1024;
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

int GetRecordDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& recordData)
{
	struct soap *s = soap_new();

	ns__Timestamp ts(1, 5);
	xsd__base64Binary raw;

	std::vector<ns__KeyValue> requestParams;
	ParamMapToVector(params, requestParams);

	int ret = soap_call_ns__RecordDataObjectRequest(s, URL.c_str(), NULL, deviceId, dataObjectURI, ts, &requestParams, raw);

	recordData = string((char *)raw.getPtr(), raw.getSize());

 	soap_end(s);
	soap_free(s); 

	return ret == SOAP_OK;
}

int GetTextDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& textData)
{
	struct soap *s = soap_new();

	ns__Timestamp ts(1, 5);
	xsd__base64Binary raw;

	std::vector<ns__KeyValue> requestParams;
	ParamMapToVector(params, requestParams);

	int ret = soap_call_ns__TextDataObjectRequest(s, URL.c_str(), NULL, deviceId, dataObjectURI, ts, &requestParams, raw);

	textData = string((char *)raw.getPtr(), raw.getSize());

 	soap_end(s);
	soap_free(s); 

	return ret == SOAP_OK;
}

int GetTreeDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& treeData)
{
	struct soap *s = soap_new();

	ns__Timestamp ts(1, 5);
	xsd__base64Binary raw;

	std::vector<ns__KeyValue> requestParams;
	ParamMapToVector(params, requestParams);

	int ret = soap_call_ns__TreeDataObjectRequest(s, URL.c_str(), NULL, deviceId, dataObjectURI, ts, &requestParams, raw);

	treeData = string((char *)raw.getPtr(), raw.getSize());

 	soap_end(s);
	soap_free(s); 

	return ret == SOAP_OK;
}

int Test_Push(const std::string& data)
{
	struct soap *s = soap_new();

	bool isDelivered = false;

	std::vector<ns__KeyValue> requestParams;
	int ret = soap_call_ns__DataObjectPushRequest(s, "http://0.0.0.0:23456", NULL, "1", data, &requestParams, isDelivered);

	soap_end(s);
	soap_free(s);

  return ret == SOAP_OK;
}

}

}
