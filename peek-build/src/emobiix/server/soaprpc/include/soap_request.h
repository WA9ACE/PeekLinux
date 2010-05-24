#ifndef __SOAPREQUEST_H__
#define __SOAPREQUEST_H__

#include <string>
#include <vector>

namespace emobiix
{

namespace soap_request
{

bool get_authentication(const std::string& uri, const std::string& user, const std::string& pass);
int get_blockDataObject(const std::string& uri, int id, char **blockData);
int get_recordDataObject(const std::string& uri, int id, std::vector<char *>& recordData);
int get_textDataObject(const std::string& uri, int id, char*& textData);
int get_treeDataObject(const std::string& uri, const std::string& id,std::string& treeData);

};

}

#endif // __SOAPREQUEST_H__
