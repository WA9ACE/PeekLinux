#include "soap_request.h"

#include "emobiix_rpc_H.h"
#include "emobiix.nsmap"

using namespace std;

namespace emobiix
{

namespace soap_request
{

bool get_authentication(const std::string& uri, const std::string& user, const std::string& pass) 
{ 
	struct soap *s = soap_new();

	bool isAuth = false;
	int ret = soap_call_ns__AuthenticationRequest(s, uri.c_str(), NULL, const_cast<char *>(user.c_str()), const_cast<char *>(pass.c_str()), isAuth);

 	soap_end(s);
	soap_free(s); 

	return isAuth;
}

int get_blockDataObject(const std::string& uri, int id, vector<pair<size_t, unsigned char *> >& blockData)
{
	struct soap *s = soap_new();

	vector<char *> blocks;

	ns__Timestamp ts(1, 5);

	xsd__base64Binary raw; 
	int ret = soap_call_ns__BlockDataObjectRequest(s, uri.c_str(), NULL, 1, ts, raw);

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

 	soap_end(s);
	return 1;
}

int get_recordDataObject(const std::string& uri, int id, std::vector<char *>& recordData)
{
	return 1;
}

int get_textDataObject(const std::string& uri, int id, char*& textData)
{
	struct soap *s = soap_new();

	ns__Timestamp ts(1, 5);
	int ret = soap_call_ns__TextDataObjectRequest(s, uri.c_str(), NULL, id, ts, textData);

 	soap_end(s);
	soap_free(s); 

	return ret;
}

int get_treeDataObject(const std::string& uri, const std::string& id, std::string& treeData)
{
	struct soap *s = soap_new();

	ns__Timestamp ts(1, 5);
	char *tree = NULL;
	int ret = soap_call_ns__TreeDataObjectRequest(s, uri.c_str(), NULL, const_cast<char *>(id.c_str()), ts, tree);

	treeData = tree;

 	soap_end(s);
	soap_free(s); 

	return 1;
}

}

}
