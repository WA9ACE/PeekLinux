#ifndef __EMOBIIX_FONT__
#define __EMOBIIX_FONT__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

FT_Library* init_freetype(void);

#ifdef __cplusplus
}
#endif

#endif // __EMOBIIX_FONT__

