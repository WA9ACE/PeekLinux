#ifndef _POINT_H_
#define _POINT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Point2_t Point;
typedef struct Point2_t Point2;
struct Point2_t {
    float x, y;
};

typedef struct IPoint2_t IPoint;
typedef struct IPoint2_t IPoint2;
struct IPoint2_t {
    int x, y;
};

typedef struct Point3_t Point3;
struct Point3_t {
    float x, y, z;
};

void point2_zero(Point2 *p);

#ifdef __cplusplus
}
#endif

#endif /* _POINT_H_ */
