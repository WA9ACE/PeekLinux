#ifndef __SOAPREQUEST_H__
#define __SOAPREQUEST_H__

#include <string>
#include <vector>
#include <utility>

namespace emobiix
{

namespace soap_request
{

bool GetAuthentication(const std::string& uri, const std::string& devId, const std::string& user, const std::string& pass);
int GetBlockDataObject(const std::string& uri, const std::string& id, std::string& mime, std::vector<std::pair<size_t, unsigned char *> >& blockData);
int GetRecordDataObject(const std::string& uri, int id, std::vector<char *>& recordData);
int GetTextDataObject(const std::string& uri, int id, char*& textData);
int GetTreeDataObject(const std::string& uri, const std::string& id,std::string& treeData);
int Test_Push(const std::string& data);

};

}

#endif // __SOAPREQUEST_H__
