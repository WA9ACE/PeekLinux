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

ns__Timestamp::ns__Timestamp(int ma, int mi)
: stampMajor(ma), stampMinor(mi)
{
}
