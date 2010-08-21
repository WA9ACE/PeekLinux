#include <strings.h>

float strtof(const char *nptr, char **endptr)
{
	if (endptr)
		*endptr = (char *)(nptr + strlen(nptr));

	return atof(nptr);
}
