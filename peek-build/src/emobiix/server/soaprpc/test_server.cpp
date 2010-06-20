#include <cstdio>

#include "stdsoap2.h"
#include "emobiix_rpc_Stub.h"
#include "emobiix_rpc_emobiixObject.h" // get server object
#include "emobiix.nsmap" // get namespace bindings

#include <string>  
#include <iostream>  
#include "curl/curl.h"  
  
using namespace std;

int main()
{
   emobiixService c;
   return c.serve(); // calls soap_serve to serve as CGI application (using stdin/out)
}

int ns__AuthenticationRequest(struct soap* soap, xsd__string devId, xsd__string user, xsd__string password, bool &isAuthenticated)
{
	cerr << "Received authentication request for [" << user << "], [" << password << "]" << endl;

	if (!strcmp(user, "peek") && !strcmp(password, "peek123"))
		isAuthenticated = true;
	else
		isAuthenticated = false;

	cerr << "Authenticated? " << isAuthenticated << endl;

	return SOAP_OK;
}

// Write any errors in here  
static char errorBuffer[CURL_ERROR_SIZE];  
 
#if 0 
// Write all expected data in here  
static string buffer;  
  
// This is the writer call back function used by curl  
static int writer(char *data, size_t size, size_t nmemb,  
                  std::string *buffer)  
{  
  // What we will return  
  int result = 0;  
  
  // Is there anything in the buffer?  
  if (buffer != NULL)  
  {  
    // Append the data to the buffer  
    buffer->append(data, size * nmemb);  
  
    // How much did we write?  
    result = size * nmemb;  
  }  
  
  return result;  
}  
 
#endif
 
bool curl_get_file(const char *szFileName, const char *szUrl)
{
	// Our curl objects  
	CURL *curl;  
	CURLcode result;  

	// Create our curl handle  
	curl = curl_easy_init();  
	if (!curl) 
		return false;

	FILE *file = fopen(szFileName, "w");
	if (!file)
		return false;

	// Now set up all of the curl options  
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);  
	curl_easy_setopt(curl, CURLOPT_URL, szUrl);  
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);  

	// Attempt to retrieve the remote page  
	result = curl_easy_perform(curl);  

	// Always cleanup  
	curl_easy_cleanup(curl);  

	fclose(file);

	// Did we succeed?  
	if (result != CURLE_OK)  
	{  
		cout << "Error: [" << result << "] - " << errorBuffer;  
		return false;
	}

	return true;
}  

int ns__BlockDataObjectRequest(struct soap* soap, xsd__string id, ns__Timestamp timestamp, xsd__base64Binary &rawData)
{
	if (!strcmp(id, "map.png"))
	{
		if (!curl_get_file(id, "http://maps.google.com/maps/api/staticmap?center=Brooklyn+Bridge,New+York,NY&zoom=14&size=320x240&maptype=roadmap&markers=color:blue|label:S|40.702147,-74.015794&markers=color:green|label:G|40.711614,-74.012318&markers=color:red|color:red|label:C|40.718217,-73.998284&sensor=false"))
		{
			return 404;
		}
	}

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

