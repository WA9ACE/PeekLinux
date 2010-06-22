#include "emobiix_rpc_H.h"

xsd__base64Binary::xsd__base64Binary()
: __ptr(NULL), __size(0), mime("unknown")
{
}

xsd__base64Binary::xsd__base64Binary(struct soap *soap, int n, const xsd__string& mimeType)
{
  __ptr = (unsigned char*)soap_malloc(soap, n);
  __size = n;
	mime = mimeType;
}

unsigned char *xsd__base64Binary::getPtr()
{
  return __ptr;
}

int xsd__base64Binary::getSize() const
{
  return __size;
}

const xsd__string& xsd__base64Binary::getMime() 
{
	return mime;
}

recordArray::recordArray()
: __ptr(NULL), __size(0)
{
}

recordArray::recordArray(struct soap* soap, int n)
{
	__ptr = (xsd__string *)soap_malloc(soap, n * sizeof(xsd__string));
	__size = n;
}

int recordArray::getSize() const 
{
	return __size;
}

xsd__string &recordArray::operator[](int i)
{
	return __ptr[i];
}

ns__Timestamp::ns__Timestamp(int ma, int mi)
: stampMajor(ma), stampMinor(mi)
{
}
