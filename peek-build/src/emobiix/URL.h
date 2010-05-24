#ifndef _URL_H_
#define _URL_H_

#ifdef __cplusplus
extern "C" {
#endif

#if 0
foo://username:password@example.com:8042/over/there/index.dtb?type=animal;name=ferret#nose
  \ /   \_______________/ \_________/ \__/            \___/ \_/ \_____________________/ \__/
   |           |               |       |                |    |           |                |
   |       userinfo         hostname  port              |    |         query          fragment
   |    \_______________________________/ \_____________|____|/
scheme                  |                               | |  |
   |                authority                           |path|
   |                                                    |    |
   |            path                       interpretable as filename
   |   ___________|____________                              |
  / \ /                        \                             |
  urn:example:animal:ferret:nose               interpretable as extension
#endif

struct URL_t {
	char *scheme;
	char *authority;
	char *hostname;
	char *port;
	char *user;
	char *password;
	char *path;
	char *query;
	char *fragment;

	char *all;
};
typedef struct URL_t URL;

#define URL_SCHEME		0x001
#define URL_AUTHORITY	0x002
#define URL_HOSTNAME	0x004
#define URL_PORT		0x008
#define URL_USERINFO	0x010
#define URL_USER		0x020
#define URL_PASSWORD	0x040
#define URL_PATH		0x080
#define URL_QUERY		0x100
#define URL_FRAGMENT	0x200
#define URL_ALL			0x3FF

URL *url_parse(const char *URL, unsigned int parts);
void url_delete(URL *u);

#ifdef __cplusplus
}
#endif

#endif /* _URL_H_ */
