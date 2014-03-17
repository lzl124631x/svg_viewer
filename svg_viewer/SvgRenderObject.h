#ifndef SvgRenderObject_h
#define SvgRenderObject_h
/*
*此接口提供创建和析构渲染对象的方法。
*/
DECLS_BEGIN
//GDI+
#include <gdiplus.h> 
using namespace Gdiplus;
#pragma comment(lib,"GdiPlus")

#include "SvgDom.h"

//the rectangle of client area
extern Rect g_client_area;
//the calculated size of view port
extern SizeF g_viewport_size;

typedef enum _SvgTagType{
	SVG_TAG_TYPE_UNKNOWN,
	SVG_TAG_TYPE_SVG,
	SVG_TAG_TYPE_DEFS,
	SVG_TAG_TYPE_G,
	SVG_TAG_TYPE_DESC,
	SVG_TAG_TYPE_TITLE,
	SVG_TAG_TYPE_RECT,
	SVG_TAG_TYPE_CIRCLE,
	SVG_TAG_TYPE_ELLIPSE,
	SVG_TAG_TYPE_LINE,
	SVG_TAG_TYPE_POLYLINE,
	SVG_TAG_TYPE_POLYGON,
	SVG_TAG_TYPE_PATH,
	SVG_TAG_TYPE_USE,
	SVG_TAG_TYPE_STYLE,
	SVG_TAG_TYPE_TEXT,
	SVG_TAG_TYPE_CLIP_PATH
}SvgTagType;

enum _StrokeLineCap;
typedef enum _StrokeLineCap StrokeLineCap;

enum _StrokeLineJoin;
typedef enum _StrokeLineJoin StrokeLineJoin;

enum _FillRule;
typedef enum _FillRule FillRule;

typedef struct _PathCommand{
	TCHAR cmd_type;
	ARRAY/* NUMBER */ data_array;
}PathCommand;

struct _SvgRenderTree;
typedef struct _SvgRenderTree SvgRenderTree;

struct _SvgRenderObject;
typedef struct _SvgRenderObject SvgRenderObject;

typedef struct _ViewBox{
	NUMBER min_x;
	NUMBER min_y;
	NUMBER width;
	NUMBER height;
}ViewBox;

//preserveAspectRatio区分大小写
typedef enum _PRESERVE_ASPECT_RATIO_ALIGN{
	PRESERVE_ASPECT_RATIO_ALIGN_NONE,
	PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMIN,
	PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMIN,
	PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMIN,
	PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMID,
	PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMID,		/* DEFAULT */
	PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMID,
	PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMAX,
	PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMAX,
	PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMAX
}PRESERVE_ASPECT_RATIO_ALIGN;

typedef enum _PRESERVE_ASPECT_RATIO_MORS{
	PRESERVE_ASPECT_RATIO_MORS_MEET,			/* DEFAULT */
	PRESERVE_ASPECT_RATIO_MORS_SLICE
}PRESERVE_ASPECT_RATIO_MORS;

typedef struct _PRESERVE_ASPECT_RATIO{
	PRESERVE_ASPECT_RATIO_ALIGN align;
	PRESERVE_ASPECT_RATIO_MORS meet_or_slice;
}PRESERVE_ASPECT_RATIO;

struct _SvgSvgRenderObject;
typedef struct _SvgSvgRenderObject SvgSvgRenderObject;

struct _SvgGRenderObject;
typedef struct _SvgGRenderObject SvgGRenderObject;

struct _SvgDefsRenderObject;
typedef struct _SvgDefsRenderObject SvgDefsRenderObject;

struct _SvgDescRenderObject;
typedef struct _SvgDescRenderObject SvgDescRenderObject;

struct _SvgTitleRenderObject;
typedef struct _SvgTitleRenderObject SvgTitleRenderObject;

struct _SvgPathRenderObject;
typedef struct _SvgPathRenderObject SvgPathRenderObject; 

struct _SvgRectRenderObject;
typedef struct _SvgRectRenderObject SvgRectRenderObject;

struct _SvgCircleRenderObject;
typedef struct _SvgCircleRenderObject SvgCircleRenderObject;

struct _SvgEllipseRenderObject;
typedef struct _SvgEllipseRenderObject SvgEllipseRenderObject;

struct _SvgLineRenderObject;
typedef struct _SvgLineRenderObject SvgLineRenderObject;

struct _SvgPolylineRenderObject;
typedef struct _SvgPolylineRenderObject SvgPolylineRenderObject;

struct _SvgPolygonRenderObject;
typedef struct _SvgPolygonRenderObject SvgPolygonRenderObject;

struct _SvgUseRenderObject;
typedef struct _SvgUseRenderObject SvgUseRenderObject;

struct _SvgStyleRenderObject;
typedef struct _SvgStyleRenderObject SvgStyleRenderObject;

struct _SvgTextRenderObject;
typedef struct _SvgTextRenderObject SvgTextRenderObject;

struct _SvgClipPathRenderObject;
typedef struct _SvgClipPathRenderObject SvgClipPathRenderObject;

//为渲染对象添加子对象
PUBLIC void svg_render_object_append_child(SvgRenderObject* thiz, SvgRenderObject* child);
//获取渲染对象的类型
PUBLIC SvgTagType svg_render_object_get_type(SvgRenderObject* thiz);
//获取渲染对象的父对象
PUBLIC SvgRenderObject* svg_render_object_get_parent(SvgRenderObject* thiz);
//获取渲染对象的首个子对象
PUBLIC SvgRenderObject* svg_render_object_get_first_child(SvgRenderObject* thiz);
//获取渲染对象的下一个兄弟对象
PUBLIC SvgRenderObject* svg_render_object_get_next_sibling(SvgRenderObject* thiz);
//判断渲染对象是否填充
PUBLIC BOOL svg_render_object_has_fill(SvgRenderObject* thiz);
//获取渲染对象的填充颜色
PUBLIC COLORREF svg_render_object_get_fill_color(SvgRenderObject* thiz);
//获取渲染对象的填充透明度
PUBLIC NUMBER svg_render_object_get_fill_opacity(SvgRenderObject* thiz);
//获取渲染对象的填充规则
PUBLIC FillMode svg_render_object_get_fill_rule(SvgRenderObject* thiz);
//判断渲染对象是否绘制边界
PUBLIC BOOL svg_render_object_has_stroke(SvgRenderObject* thiz);
//获取渲染对象的边界宽度
PUBLIC NUMBER svg_render_object_get_stroke_width(SvgRenderObject* thiz);
//获取渲染对象的边界颜色
PUBLIC COLORREF svg_render_object_get_stroke_color(SvgRenderObject* thiz);
//获取渲染对象的边界线帽
PUBLIC LineCap svg_render_object_get_stroke_linecap(SvgRenderObject* thiz);
//获取渲染对象的边界连接点类型
PUBLIC LineJoin svg_render_object_get_stroke_linejoin(SvgRenderObject* thiz);
//获取渲染对象的边界透明度
PUBLIC NUMBER svg_render_object_get_stroke_opacity(SvgRenderObject* thiz);
//获取渲染对象的边界虚线数组
PUBLIC ARRAY svg_render_object_get_stroke_dasharray(SvgRenderObject* thiz);
//获取渲染对象的边界虚线偏移
PUBLIC NUMBER svg_render_object_get_stroke_dashoffset(SvgRenderObject* thiz);
//获取渲染对象的边界尖角限量
PUBLIC NUMBER svg_render_object_get_stroke_miterlimit(SvgRenderObject* thiz);
//获取渲染对象的变换矩阵
PUBLIC NUMBER* svg_render_object_get_transform(SvgRenderObject* thiz);
//获取渲染对象的剪裁路径名
PUBLIC LPCTSTR svg_render_object_get_clip_path_name(SvgRenderObject* thiz);
//获取渲染对象的建材路径对象
PUBLIC SvgClipPathRenderObject* svg_render_object_get_clip_path_element(SvgRenderObject* thiz);
//设置渲染对象的剪裁路径对象
PUBLIC void svg_render_object_set_clip_path_element(SvgRenderObject* thiz, SvgClipPathRenderObject* element);


//创建渲染树
PUBLIC SvgRenderTree* svg_render_tree_create();
//析构渲染树
PUBLIC void svg_render_tree_destroy(SvgRenderTree* thiz);
//获取渲染树的根对象
PUBLIC SvgRenderObject* svg_render_tree_get_root(SvgRenderTree* thiz);
//设置渲染树的根对象
PUBLIC void svg_render_tree_set_root(SvgRenderTree* thiz, SvgRenderObject* root);


//将字符串转化为渲染对象类型
PUBLIC SvgTagType string_to_svg_render_object_type(LPCTSTR str);
//根据Dom节点创建对应的渲染对象（工厂模式）。
PUBLIC SvgRenderObject* svg_render_object_factory_create_render_object(DomNode* node, SvgRenderObject* parent);

//创建路径渲染对象
PUBLIC SvgPathRenderObject* svg_path_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建矩形渲染对象
PUBLIC SvgRectRenderObject* svg_rect_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建圆形渲染对象
PUBLIC SvgCircleRenderObject* svg_circle_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建椭圆形渲染对象
PUBLIC SvgEllipseRenderObject* svg_ellipse_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建线形渲染对象
PUBLIC SvgLineRenderObject* svg_line_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建折线渲染对象
PUBLIC SvgPolylineRenderObject* svg_polyline_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建多边形渲染对象
PUBLIC SvgPolygonRenderObject* svg_polygon_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建Svg渲染对象
PUBLIC SvgSvgRenderObject* svg_svg_render_object_create(DomNode* dom_node);
//创建组合渲染对象
PUBLIC SvgGRenderObject* svg_g_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建引用渲染对象
PUBLIC SvgUseRenderObject* svg_use_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//创建剪取路径渲染对象
PUBLIC SvgClipPathRenderObject* svg_clip_path_render_object_create(DomNode* dom_node);

//获取Svg渲染对象的viewBox属性
PUBLIC ViewBox svg_svg_render_object_get_view_box(SvgSvgRenderObject* thiz);
//判断Svg渲染对象是否有viewBox属性
PUBLIC BOOL svg_svg_render_object_has_view_box(SvgSvgRenderObject* thiz);
//获取Svg渲染对象的宽度
PUBLIC LENGTH svg_svg_render_object_get_width(SvgSvgRenderObject* thiz);
//获取Svg渲染对象的高度
PUBLIC LENGTH svg_svg_render_object_get_height(SvgSvgRenderObject* thiz);
//获取Svg渲染对象的preserveAspectRatio属性
PUBLIC PRESERVE_ASPECT_RATIO svg_svg_render_object_get_preserve_aspect_ratio(SvgSvgRenderObject* thiz);

//获取路径渲染对象的路径数据
PUBLIC ARRAY svg_path_render_object_get_path_data(SvgPathRenderObject* thiz);

//获取矩形渲染对象的左上角横坐标
PUBLIC COORDINATE svg_rect_render_object_get_x(SvgRectRenderObject* thiz);
//获取矩形渲染对象的左上角纵坐标
PUBLIC COORDINATE svg_rect_render_object_get_y(SvgRectRenderObject* thiz);
//获取矩形渲染对象的宽度
PUBLIC LENGTH svg_rect_render_object_get_width(SvgRectRenderObject* thiz);
//获取矩形渲染对象的高度
PUBLIC LENGTH svg_rect_render_object_get_height(SvgRectRenderObject* thiz);
//获取矩形渲染对象的圆角x半径
PUBLIC LENGTH svg_rect_render_object_get_rx(SvgRectRenderObject* thiz);
//获取矩形渲染对象的圆角y半径
PUBLIC LENGTH svg_rect_render_object_get_ry(SvgRectRenderObject* thiz);

//获取圆形渲染对象的圆心横坐标
PUBLIC NUMBER svg_circle_render_object_get_center_x(SvgCircleRenderObject* thiz);
//获取圆形渲染对象的圆心纵坐标
PUBLIC NUMBER svg_circle_render_object_get_center_y(SvgCircleRenderObject* thiz);
//获取圆形渲染对象的半径
PUBLIC NUMBER svg_circle_render_object_get_radius(SvgCircleRenderObject* thiz);

//获取椭圆形渲染对象的圆心横坐标
PUBLIC NUMBER svg_ellipse_render_object_get_center_x(SvgEllipseRenderObject* thiz);
//获取椭圆形渲染对象的圆心纵坐标
PUBLIC NUMBER svg_ellipse_render_object_get_center_y(SvgEllipseRenderObject* thiz);
//获取椭圆形渲染对象的半长轴
PUBLIC NUMBER svg_ellipse_render_object_get_radius_x(SvgEllipseRenderObject* thiz);
//获取椭圆形渲染对象的半短轴
PUBLIC NUMBER svg_ellipse_render_object_get_radius_y(SvgEllipseRenderObject* thiz);

//获取线形渲染对象的起始点横坐标
PUBLIC NUMBER svg_line_render_object_get_x1(SvgLineRenderObject* thiz);
//获取线形渲染对象的起始点纵坐标
PUBLIC NUMBER svg_line_render_object_get_y1(SvgLineRenderObject* thiz);
//获取线形渲染对象的结束点横坐标
PUBLIC NUMBER svg_line_render_object_get_x2(SvgLineRenderObject* thiz);
//获取线形渲染对象的结束点纵坐标
PUBLIC NUMBER svg_line_render_object_get_y2(SvgLineRenderObject* thiz);

//获取折线渲染对象的点集
PUBLIC ARRAY svg_polyline_render_object_get_point_array(SvgPolylineRenderObject* thiz);

//获取多边形渲染对象的点集
PUBLIC ARRAY svg_polygon_render_object_get_point_array(SvgPolygonRenderObject* thiz);

//获取引用渲染对象的xlink:href属性
PUBLIC LPCTSTR svg_use_render_object_get_xlink_href_name(SvgUseRenderObject* thiz);
//获取引用渲染对象的引用DomElement
PUBLIC DomElement* svg_use_render_object_get_xlink_href_element(SvgUseRenderObject* thiz);
//设置引用渲染对象的引用DomElement
PUBLIC void svg_use_render_object_set_xlink_href_element(SvgUseRenderObject* thiz, DomElement* dom_element);
//获取引用渲染对象的横坐标
PUBLIC NUMBER svg_use_render_object_get_x(SvgUseRenderObject* thiz);
//获取引用渲染对象的纵坐标
PUBLIC NUMBER svg_use_render_object_get_y(SvgUseRenderObject* thiz);

DECLS_END
#endif