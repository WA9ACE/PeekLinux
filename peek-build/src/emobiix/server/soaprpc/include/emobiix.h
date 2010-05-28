//gsoap ns service name: emobiix
//gsoap ns service encoding: literal 

#import "stl.h"

class xsd__base64Binary
{
public:
	unsigned char *__ptr;
	int __size;

	xsd__base64Binary();
	xsd__base64Binary(struct soap* soap, int n);
	int getSize() const;
	unsigned char *getPtr();
};

typedef char *XML;
typedef char *xsd__string;

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

int ns__AuthenticationRequest(xsd__string user, xsd__string pass, bool &isAuthenticated);
int ns__BlockDataObjectRequest(int id, ns__Timestamp timestamp, xsd__base64Binary &blockData);
int ns__TreeDataObjectRequest(xsd__string id, ns__Timestamp timestamp, XML& m__treeData);
int ns__RecordDataObjectRequest(int id, ns__Timestamp timestamp, recordArray &recordData);
int ns__TextDataObjectRequest(int id, ns__Timestamp timestamp, xsd__string& textData);
