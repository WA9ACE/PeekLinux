#ifndef _RENDERDRIVER_H_
#define _RENDERDRIVER_H_

#include "Gradient.h"

#include <stdint.h>

typedef struct RenderDriver_t RenderDriver;
struct {
    void (*fill)(RenderContext *);
    void (*stroke)(RenderContext *);
    void (*clip)(RenderContext *);

    void (*move_to)(RenderContext *,float,float);
    void (*line_to)(RenderContext *,float,float);
    void (*hline)(RenderContext *,float);
    void (*vline)(RenderContext *,float);
    void (*close_path)(RenderContext *);
    void (*rectangle)(RenderContext *,float,float,float,float);
    void (*arc)(RenderContext *,float,float,float,float,float);
    void (*path_preserve)(RenderContext *);

    void (*set_linewidth)(RenderContext *, float);

    void (*set_rgba)(RenderContext *, uint8_t, uint8_t, uint8_t, uint8_t);
    void (*set_rgb)(RenderContext *, uint8_t, uint8_t, uint8_t);

    void (*gradient_linear)(RenderContext *, Gradient *, float, float,
            float, float);
    void (*gradient_stop)(RenderContext *, Gradient *, float,
            uint8_t, uint8_t, uint8_t);
} RenderDriver_t;

#endif /* _RENDERDRIVER_H_ */
