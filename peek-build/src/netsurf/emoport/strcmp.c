typedef unsigned int size_t;

int strcasecmp(const char *s1, const char *s2)
{
       const unsigned char *us1 = (const unsigned char *)s1, *us2 = (const unsigned char *)s2;

       while (tolower(*us1) == tolower(*us2)) {
               if (*us1++ == '\0')
                       return (0);
        us2++;
       }
       return (tolower(*us1) - tolower(*us2));

}
int strncasecmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char *us1;
	const unsigned char *us2;
        if (n != 0) {
               us1 = (const unsigned char *)s1;
               us2 = (const unsigned char *)s2;

                do {
                        if (tolower(*us1) != tolower(*us2))
                                return (tolower(*us1) - tolower(*us2));
                        if (*us1++ == '\0')
                                break;
                        us2++;
                } while (--n != 0);
        }
        return (0);
}
