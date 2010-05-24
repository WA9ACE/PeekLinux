#include "Cache.h"

#include "Platform.h"

#include "p_malloc.h"

#include <string.h>

/*
	Cache -
	/base/url.file.name			<- object data
	/base/url.file.name.client	<- client side modifications
	/base/url.file.name.server	<- changed from server not yet applied
*/

static const char *CLIENT_EXTENSION = "client";

static char *cache_URLFilename(URL *url, const char *extension);

void cache_init(void)
{

}

DataObject *cache_loadObject(URL *url)
{
	char *fileName;
	char *fileNameClient;

	fileName = cache_URLFilename(url, NULL);
	fileNameClient = cache_URLFilename(url, CLIENT_EXTENSION);
	return NULL;
}

int cache_addToObject(DataObject *obj, int fromServer,
		void *buffer, int len)
{
	return 0;
}

int cache_commitObject(DataObject *dobj)
{
	return 0;
}

static char *cache_URLFilename(URL *url, const char *extension)
{
	int slen, fieldCount;
	char *output;

	if (url == NULL)
		return NULL;

	slen = 0;
	fieldCount = 0;
#define FIELD_LEN(_f) \
if (_f != NULL) { \
	slen += strlen(_f)+1; \
	++fieldCount; \
}

	FIELD_LEN(url->scheme);
	FIELD_LEN(url->hostname);
	FIELD_LEN(url->port);
	FIELD_LEN(url->path);
	FIELD_LEN(extension);

#undef FIELD_LEN

	output = p_malloc(slen);
	if (output == NULL)
		return NULL;

#define FIELD_COPY(_f) \
	if (_f != NULL) { \
		strcpy(output, _f); \
		--fieldCount; \
		if (fieldCount) \
			strcpy(output, "."); \
	}

	FIELD_COPY(url->scheme);
	FIELD_COPY(url->hostname);
	FIELD_COPY(url->port);
	FIELD_COPY(url->path);
	FIELD_COPY(extension);

	return output;
}
