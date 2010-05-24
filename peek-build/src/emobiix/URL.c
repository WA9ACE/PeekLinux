#include "URL.h"

#include "p_malloc.h"

#include <string.h>

URL *url_parse(const char *url, unsigned int parts)
{
	URL *output;
	unsigned int done;
	const char *p1, *p2, *p3;

	if (url == NULL || parts == 0)
		return NULL;

	done = 0;
	p1 = url;
	p2 = strchr(p1, ':');
	if (p2 == NULL)
		return NULL;

	output = (URL *)p_malloc(sizeof(URL));
	memset(output, 0, sizeof(URL));

	output->all = p_strdup(url);

	if (parts & URL_SCHEME) {
		output->scheme = p_malloc(p2-p1+1);
		memcpy(output->scheme, p1, p2-p1);
		output->scheme[p2-p1] = 0;
		
		done |= URL_SCHEME;

		if (done == parts)
			return output;
	}

	p1 = p2;
	p1 += 3;
	p2 = strchr(p1, '/');
	if (p2 == NULL) {
		url_delete(output);
		return NULL;
	}

	if (parts & URL_AUTHORITY) {
		output->authority = p_malloc(p2-p1+1);
		memcpy(output->authority, p1, p2-p1);
		output->authority[p2-p1] = 0;
		
		done |= URL_AUTHORITY;

		if (done == parts)
			return output;
	}

	p3 = strchr(p1 ,'@');
	if (p3 == NULL)
		goto process_hostname;

	/* just ignore user:pass for now */

process_hostname:
	p2 = strchr(p1, ':');
	if (p2 == NULL) {
		p2 = strchr(p1, '/');
		if (p2 == NULL) {
			url_delete(output);
			return NULL;
		}
	}

	if (parts & URL_HOSTNAME) {
		output->hostname = p_malloc(p2-p1+1);
		memcpy(output->hostname, p1, p2-p1);
		output->hostname[p2-p1] = 0;
		
		done |= URL_HOSTNAME;

		if (done == parts)
			return output;
	}

	if (*p2 == '/')
		goto process_path;

	p1 = p2+1;
	p2 = strchr(p1, '/');
	
	if (parts & URL_PORT) {
		output->port = p_malloc(p2-p1+1);
		memcpy(output->port, p1, p2-p1);
		output->port[p2-p1] = 0;
		
		done |= URL_PORT;

		if (done == parts)
			return output;
	}

process_path:

	/* just process all as path for now */

	p1 = p2;
	if (parts & URL_PATH) {
		output->path = p_strdup(p1);
		
		done |= URL_PATH;

		if (done == parts)
			return output;
	}

	return output;
}

void url_delete(URL *u)
{
#define FREE_FIELD(__x) if (u->__x != NULL) p_free(u->__x)
	FREE_FIELD(all);
	FREE_FIELD(scheme);
	FREE_FIELD(authority);
	FREE_FIELD(hostname);
	FREE_FIELD(port);
	FREE_FIELD(user);
	FREE_FIELD(password);
	FREE_FIELD(path);
	FREE_FIELD(query);
	FREE_FIELD(fragment);
#undef FREE_FIELD

	p_free(u);
}
