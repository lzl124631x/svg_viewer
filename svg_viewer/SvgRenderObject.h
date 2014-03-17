#ifndef SvgRenderObject_h
#define SvgRenderObject_h
/*
*�˽ӿ��ṩ������������Ⱦ����ķ�����
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

//preserveAspectRatio���ִ�Сд
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

//Ϊ��Ⱦ��������Ӷ���
PUBLIC void svg_render_object_append_child(SvgRenderObject* thiz, SvgRenderObject* child);
//��ȡ��Ⱦ���������
PUBLIC SvgTagType svg_render_object_get_type(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ĸ�����
PUBLIC SvgRenderObject* svg_render_object_get_parent(SvgRenderObject* thiz);
//��ȡ��Ⱦ������׸��Ӷ���
PUBLIC SvgRenderObject* svg_render_object_get_first_child(SvgRenderObject* thiz);
//��ȡ��Ⱦ�������һ���ֵܶ���
PUBLIC SvgRenderObject* svg_render_object_get_next_sibling(SvgRenderObject* thiz);
//�ж���Ⱦ�����Ƿ����
PUBLIC BOOL svg_render_object_has_fill(SvgRenderObject* thiz);
//��ȡ��Ⱦ����������ɫ
PUBLIC COLORREF svg_render_object_get_fill_color(SvgRenderObject* thiz);
//��ȡ��Ⱦ��������͸����
PUBLIC NUMBER svg_render_object_get_fill_opacity(SvgRenderObject* thiz);
//��ȡ��Ⱦ�����������
PUBLIC FillMode svg_render_object_get_fill_rule(SvgRenderObject* thiz);
//�ж���Ⱦ�����Ƿ���Ʊ߽�
PUBLIC BOOL svg_render_object_has_stroke(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽���
PUBLIC NUMBER svg_render_object_get_stroke_width(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽���ɫ
PUBLIC COLORREF svg_render_object_get_stroke_color(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽���ñ
PUBLIC LineCap svg_render_object_get_stroke_linecap(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽����ӵ�����
PUBLIC LineJoin svg_render_object_get_stroke_linejoin(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽�͸����
PUBLIC NUMBER svg_render_object_get_stroke_opacity(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽���������
PUBLIC ARRAY svg_render_object_get_stroke_dasharray(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽�����ƫ��
PUBLIC NUMBER svg_render_object_get_stroke_dashoffset(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı߽�������
PUBLIC NUMBER svg_render_object_get_stroke_miterlimit(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ı任����
PUBLIC NUMBER* svg_render_object_get_transform(SvgRenderObject* thiz);
//��ȡ��Ⱦ����ļ���·����
PUBLIC LPCTSTR svg_render_object_get_clip_path_name(SvgRenderObject* thiz);
//��ȡ��Ⱦ����Ľ���·������
PUBLIC SvgClipPathRenderObject* svg_render_object_get_clip_path_element(SvgRenderObject* thiz);
//������Ⱦ����ļ���·������
PUBLIC void svg_render_object_set_clip_path_element(SvgRenderObject* thiz, SvgClipPathRenderObject* element);


//������Ⱦ��
PUBLIC SvgRenderTree* svg_render_tree_create();
//������Ⱦ��
PUBLIC void svg_render_tree_destroy(SvgRenderTree* thiz);
//��ȡ��Ⱦ���ĸ�����
PUBLIC SvgRenderObject* svg_render_tree_get_root(SvgRenderTree* thiz);
//������Ⱦ���ĸ�����
PUBLIC void svg_render_tree_set_root(SvgRenderTree* thiz, SvgRenderObject* root);


//���ַ���ת��Ϊ��Ⱦ��������
PUBLIC SvgTagType string_to_svg_render_object_type(LPCTSTR str);
//����Dom�ڵ㴴����Ӧ����Ⱦ���󣨹���ģʽ����
PUBLIC SvgRenderObject* svg_render_object_factory_create_render_object(DomNode* node, SvgRenderObject* parent);

//����·����Ⱦ����
PUBLIC SvgPathRenderObject* svg_path_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//����������Ⱦ����
PUBLIC SvgRectRenderObject* svg_rect_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//����Բ����Ⱦ����
PUBLIC SvgCircleRenderObject* svg_circle_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//������Բ����Ⱦ����
PUBLIC SvgEllipseRenderObject* svg_ellipse_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//����������Ⱦ����
PUBLIC SvgLineRenderObject* svg_line_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//����������Ⱦ����
PUBLIC SvgPolylineRenderObject* svg_polyline_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//�����������Ⱦ����
PUBLIC SvgPolygonRenderObject* svg_polygon_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//����Svg��Ⱦ����
PUBLIC SvgSvgRenderObject* svg_svg_render_object_create(DomNode* dom_node);
//���������Ⱦ����
PUBLIC SvgGRenderObject* svg_g_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//����������Ⱦ����
PUBLIC SvgUseRenderObject* svg_use_render_object_create(DomNode* dom_node, SvgRenderObject* parent);
//������ȡ·����Ⱦ����
PUBLIC SvgClipPathRenderObject* svg_clip_path_render_object_create(DomNode* dom_node);

//��ȡSvg��Ⱦ�����viewBox����
PUBLIC ViewBox svg_svg_render_object_get_view_box(SvgSvgRenderObject* thiz);
//�ж�Svg��Ⱦ�����Ƿ���viewBox����
PUBLIC BOOL svg_svg_render_object_has_view_box(SvgSvgRenderObject* thiz);
//��ȡSvg��Ⱦ����Ŀ��
PUBLIC LENGTH svg_svg_render_object_get_width(SvgSvgRenderObject* thiz);
//��ȡSvg��Ⱦ����ĸ߶�
PUBLIC LENGTH svg_svg_render_object_get_height(SvgSvgRenderObject* thiz);
//��ȡSvg��Ⱦ�����preserveAspectRatio����
PUBLIC PRESERVE_ASPECT_RATIO svg_svg_render_object_get_preserve_aspect_ratio(SvgSvgRenderObject* thiz);

//��ȡ·����Ⱦ�����·������
PUBLIC ARRAY svg_path_render_object_get_path_data(SvgPathRenderObject* thiz);

//��ȡ������Ⱦ��������ϽǺ�����
PUBLIC COORDINATE svg_rect_render_object_get_x(SvgRectRenderObject* thiz);
//��ȡ������Ⱦ��������Ͻ�������
PUBLIC COORDINATE svg_rect_render_object_get_y(SvgRectRenderObject* thiz);
//��ȡ������Ⱦ����Ŀ��
PUBLIC LENGTH svg_rect_render_object_get_width(SvgRectRenderObject* thiz);
//��ȡ������Ⱦ����ĸ߶�
PUBLIC LENGTH svg_rect_render_object_get_height(SvgRectRenderObject* thiz);
//��ȡ������Ⱦ�����Բ��x�뾶
PUBLIC LENGTH svg_rect_render_object_get_rx(SvgRectRenderObject* thiz);
//��ȡ������Ⱦ�����Բ��y�뾶
PUBLIC LENGTH svg_rect_render_object_get_ry(SvgRectRenderObject* thiz);

//��ȡԲ����Ⱦ�����Բ�ĺ�����
PUBLIC NUMBER svg_circle_render_object_get_center_x(SvgCircleRenderObject* thiz);
//��ȡԲ����Ⱦ�����Բ��������
PUBLIC NUMBER svg_circle_render_object_get_center_y(SvgCircleRenderObject* thiz);
//��ȡԲ����Ⱦ����İ뾶
PUBLIC NUMBER svg_circle_render_object_get_radius(SvgCircleRenderObject* thiz);

//��ȡ��Բ����Ⱦ�����Բ�ĺ�����
PUBLIC NUMBER svg_ellipse_render_object_get_center_x(SvgEllipseRenderObject* thiz);
//��ȡ��Բ����Ⱦ�����Բ��������
PUBLIC NUMBER svg_ellipse_render_object_get_center_y(SvgEllipseRenderObject* thiz);
//��ȡ��Բ����Ⱦ����İ볤��
PUBLIC NUMBER svg_ellipse_render_object_get_radius_x(SvgEllipseRenderObject* thiz);
//��ȡ��Բ����Ⱦ����İ����
PUBLIC NUMBER svg_ellipse_render_object_get_radius_y(SvgEllipseRenderObject* thiz);

//��ȡ������Ⱦ�������ʼ�������
PUBLIC NUMBER svg_line_render_object_get_x1(SvgLineRenderObject* thiz);
//��ȡ������Ⱦ�������ʼ��������
PUBLIC NUMBER svg_line_render_object_get_y1(SvgLineRenderObject* thiz);
//��ȡ������Ⱦ����Ľ����������
PUBLIC NUMBER svg_line_render_object_get_x2(SvgLineRenderObject* thiz);
//��ȡ������Ⱦ����Ľ�����������
PUBLIC NUMBER svg_line_render_object_get_y2(SvgLineRenderObject* thiz);

//��ȡ������Ⱦ����ĵ㼯
PUBLIC ARRAY svg_polyline_render_object_get_point_array(SvgPolylineRenderObject* thiz);

//��ȡ�������Ⱦ����ĵ㼯
PUBLIC ARRAY svg_polygon_render_object_get_point_array(SvgPolygonRenderObject* thiz);

//��ȡ������Ⱦ�����xlink:href����
PUBLIC LPCTSTR svg_use_render_object_get_xlink_href_name(SvgUseRenderObject* thiz);
//��ȡ������Ⱦ���������DomElement
PUBLIC DomElement* svg_use_render_object_get_xlink_href_element(SvgUseRenderObject* thiz);
//����������Ⱦ���������DomElement
PUBLIC void svg_use_render_object_set_xlink_href_element(SvgUseRenderObject* thiz, DomElement* dom_element);
//��ȡ������Ⱦ����ĺ�����
PUBLIC NUMBER svg_use_render_object_get_x(SvgUseRenderObject* thiz);
//��ȡ������Ⱦ�����������
PUBLIC NUMBER svg_use_render_object_get_y(SvgUseRenderObject* thiz);

DECLS_END
#endif