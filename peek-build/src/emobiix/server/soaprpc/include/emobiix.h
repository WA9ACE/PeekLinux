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

class ns__Timestamp
{
public:
	ns__Timestamp(int ma, int mi);
	int stampMajor;
	int stampMinor;
};

//gsoap ns service method-documentation: AuthenticationRequest Returns whether the given deviceId, userName, password combination is authenticated
int ns__AuthenticationRequest(
	xsd__string deviceId,
	xsd__string userName, 
	xsd__string password, 
	bool &isAuthenticated
);

//gsoap ns service method-documentation: BlockDataObjectRequest Returns the raw encoded data for a given deviceId, dataObjectURI, and timeStamp combination
int ns__BlockDataObjectRequest(
	xsd__string deviceId,
	xsd__string dataObjectURI, 
	ns__Timestamp timeStamp, 
	xsd__base64Binary &binaryData
);

//gsoap ns service method-documentation: TreeDataObjectRequest Returns the data tree for a given deviceId, dataObjectURI, and timeStamp combination
int ns__TreeDataObjectRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	ns__Timestamp timeStamp, 
	XML& m__treeData
);

//gsoap ns service method-documentation: RecordDataObjectRequest Returns the record array for a given deviceId, dataObjectURI, and timeStamp combination
int ns__RecordDataObjectRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	ns__Timestamp timestamp, 
	XML& m__recordData
);

//gsoap ns service method-documentation: TextDataObjectRequest Returns the pure textual data for a given deviceId, dataObjectURI, and timeStamp combination
int ns__TextDataObjectRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	ns__Timestamp timestamp, 
	xsd__string& textData
);

//gsoap ns service method-documentation: DataObjectPushRequest Returns whether the deviceId, dataObjectURI push request has been delivered
int ns__DataObjectPushRequest(
	xsd__string deviceId, 
	xsd__string dataObjectURI, 
	bool &isDelivered
);
