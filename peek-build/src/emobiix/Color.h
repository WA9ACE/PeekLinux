#ifndef _COLOR_H_
#define _COLOR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {RGB565, RGB, RGBA, A8, A4, RGB565A8, A8A8} PixelFormat;

union Color_t {
	unsigned int value;
	struct {
		unsigned char alpha;
		unsigned char blue;
		unsigned char green;
		unsigned char red;
	} rgba;
};
typedef union Color_t Color;

#ifdef __cplusplus
}
#endif

#endif /* _COLOR_H_ */

