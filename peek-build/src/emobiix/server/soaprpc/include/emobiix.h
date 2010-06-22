//gsoap ns service name: emobiix
//gsoap ns service encoding: literal 
//gsoap ns service documentation: emobiix mobile communication layer

#import "stl.h"

typedef std::string XML;
typedef std::string xsd__string; 

class xsd__base64Binary
{
public:
	unsigned char *__ptr;
	int __size;
	@xsd__string mime;

	xsd__base64Binary();
	xsd__base64Binary(struct soap* soap, int n, const xsd__string& mime);
	int getSize() const;
	unsigned char *getPtr();
	const xsd__string& getMime();
};

class recordArray
{
public:
	xsd__string* __ptr;
	int __size;

	recordArray();
	recordArray(struct soap* soap, int n);
	int getSize() const;
	xsd__string& operator[](int i);
};

class ns__Timestamp
{
public:
	ns__Timestamp(int ma, int mi);
	int stampMajor;
	int stampMinor;
};

int ns__AuthenticationRequest(
	xsd__string deviceId,
	xsd__string userName, 
	xsd__string password, 
	bool &isAuthenticated
);

int ns__BlockDataObjectRequest(
	xsd__string deviceId,
	xsd__string dataObjectURI, 
	ns__Timestamp timeStamp, 
	xsd__base64Binary &binaryData
);

int ns__TreeDataObjectRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	ns__Timestamp timeStamp, 
	XML& m__treeData
);

int ns__RecordDataObjectRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	ns__Timestamp timestamp, 
	recordArray &recordData
);

int ns__TextDataObjectRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	ns__Timestamp timestamp, 
	xsd__string& textData
);

int ns__DataObjectPushRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	bool &isDelivered
);
