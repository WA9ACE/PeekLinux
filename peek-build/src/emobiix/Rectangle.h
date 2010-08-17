#ifndef _RECTANGLE_H_
#define _RECTANGLE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct Rectangle_t {
    int x, y;
    int width, height;
};
typedef struct Rectangle_t Rectangle;

void rectangle_zero(Rectangle *r);
void rectangle_union(Rectangle *r, Rectangle *o);

#ifdef __cplusplus
}
#endif

#endif /* _RECTANGLE_H_ */
