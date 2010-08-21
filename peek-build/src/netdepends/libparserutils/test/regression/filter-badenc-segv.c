#include <stdio.h>
#include <stdlib.h>

#include <parserutils/parserutils.h>

#include "input/filter.h"

#include "testutils.h"

static void *myrealloc(void *ptr, size_t len, void *pw)
{
	UNUSED(pw);

	return realloc(ptr, len);
}

int main(int argc, char **argv)
{
	parserutils_filter *input;
	parserutils_filter_optparams params;
	parserutils_error expected;

#ifdef WITH_ICONV_FILTER
	expected = PARSERUTILS_OK;
#else
	expected = PARSERUTILS_BADENCODING;
#endif

	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	assert(parserutils_initialise(argv[1], myrealloc, NULL) == 
			PARSERUTILS_OK);

	assert(parserutils_filter_create("UTF-8", myrealloc, NULL, &input) ==
			PARSERUTILS_OK);

	params.encoding.name = "GBK";
	assert(parserutils_filter_setopt(input, 
			PARSERUTILS_FILTER_SET_ENCODING, &params) == 
			expected);

	params.encoding.name = "GBK";
	assert(parserutils_filter_setopt(input, 
			PARSERUTILS_FILTER_SET_ENCODING, &params) == 
			expected);

	parserutils_filter_destroy(input);

	assert(parserutils_finalise(myrealloc, NULL) == PARSERUTILS_OK);

	printf("PASS\n");

	return 0;
}
