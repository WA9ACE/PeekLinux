#ifndef __SOAPREQUEST_H__
#define __SOAPREQUEST_H__

#include <string>
#include <vector>
#include <utility>
#include <map>

namespace emobiix
{

namespace soap_request
{

bool GetAuthentication(const std::string& URL, const std::string& deviceId, const std::string& userName, const std::string& password, const std::map<std::string, std::string>& params);
int GetBlockDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& mime, std::vector<std::pair<size_t, unsigned char *> >& blockData);
int GetTextDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& textData);
int GetTreeDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& treeData);
int GetRecordDataObject(const std::string& URL, const std::string& deviceId, const std::string& dataObjectURI, const std::map<std::string, std::string>& params, std::string& recordData);
int Test_Push(const std::string& data);

};

}

#endif // __SOAPREQUEST_H__
