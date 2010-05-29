#include "emobiix_rpc_H.h"

xsd__base64Binary::xsd__base64Binary()
: __ptr(NULL), __size(0), mime("unknown")
{
}

xsd__base64Binary::xsd__base64Binary(struct soap *soap, int n, char *type)
{
  __ptr = (unsigned char*)soap_malloc(soap, n);
  __size = n;
	mime = type;
}

unsigned char *xsd__base64Binary::getPtr()
{
  return __ptr;
}

int xsd__base64Binary::getSize() const
{
  return __size;
}

char *xsd__base64Binary::getMime() 
{
	return mime;
}

recordArray::recordArray()
: __ptr(NULL), __size(0)
{
}

recordArray::recordArray(struct soap* soap, int n)
{
	__ptr = (char **)soap_malloc(soap, n * sizeof(char *));
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
