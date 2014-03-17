#ifndef SVGRenderer_GdiPlus_h
#define SVGRenderer_GdiPlus_h
DECLS_BEGIN
#include "SvgRenderObject.h"

PUBLIC extern float svg_renderer_scale_ratio;
PUBLIC void svg_renderer_render_gdiplus(SvgRenderTree* render_tree, Graphics* graphics);
DECLS_END
#endif