#include "stdafx.h"
#include "SvgRenderObject.h"
#include <math.h>//cos, sin
#define PI 3.1415926f


Rect g_client_area;
SizeF g_viewport_size;

typedef struct _Notation{
	LPTSTR mnemonic;
	int token;
}Notation;

const Notation SvgRenderObjectTypes[]={
	{_T("svg"),			SVG_TAG_TYPE_SVG},				//SVG
	{_T("defs"),		SVG_TAG_TYPE_DEFS},				//定义
	{_T("g"),			SVG_TAG_TYPE_G},				//组
	{_T("desc"),		SVG_TAG_TYPE_DESC},				//描述
	{_T("title"),		SVG_TAG_TYPE_TITLE},			//标题
	{_T("rect"),		SVG_TAG_TYPE_RECT},				//矩形
	{_T("circle"),		SVG_TAG_TYPE_CIRCLE},			//圆形
	{_T("ellipse"),		SVG_TAG_TYPE_ELLIPSE},			//椭圆
	{_T("line"),		SVG_TAG_TYPE_LINE},				//线
	{_T("polyline"),	SVG_TAG_TYPE_POLYLINE},			//折线
	{_T("polygon"),		SVG_TAG_TYPE_POLYGON},			//多边形
	{_T("path"),		SVG_TAG_TYPE_PATH},				//路径
	{_T("use"),			SVG_TAG_TYPE_USE},				//引用
	{_T("style"),		SVG_TAG_TYPE_STYLE},			//样式
	{_T("text"),		SVG_TAG_TYPE_TEXT},				//文字
	{_T("clipPath"),	SVG_TAG_TYPE_CLIP_PATH},		//剪切路径
};

//线帽类型，不区分大小写
const Notation StrokeLineCaps[] = {
	{_T("butt"),			LineCapFlat},			/* DEFAULT */
	{_T("round"),			LineCapRound},
	{_T("square"),			LineCapSquare},
};

//线条连接类型，不区分大小写
const Notation StrokeLineJoins[] = {
	{_T("miter"),			LineJoinMiter},			/* DEFAULT */
	{_T("round"),			LineJoinRound},
	{_T("bevel"),			LineJoinBevel},
};

//填充规则，不区分大小写
const Notation FillRull[] = {
	{_T("nonzero"),			FillModeWinding},		/* DEFAULT */
	{_T("evenodd"),			FillModeAlternate}
};

//保持纵横比的对齐选项，区分大小写
const Notation PreserveAspectRatioAlign[] = {
	{_T("none"),				PRESERVE_ASPECT_RATIO_ALIGN_NONE},		
	{_T("xMinYMin"),			PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMIN},
	{_T("xMidYMin"),			PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMIN},
	{_T("xMaxYMin"),			PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMIN},
	{_T("xMinYMid"),			PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMID},
	{_T("xMidYMid"),			PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMID},			/* DEFAULT */
	{_T("xMaxYMid"),			PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMID},
	{_T("xMinYMax"),			PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMAX},
	{_T("xMidYMax"),			PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMAX},
	{_T("xMaxYMax"),			PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMAX}
};

struct _SvgRenderTree{
	SvgRenderObject* root;
};

/************************************************************************/
/*                          RENDER OBJECT                               */
/************************************************************************/
typedef struct _URI{
	LPCTSTR name;
	void* target;
}URI;

//<!ENTITY % stdAttrs
//	"id ID #IMPLIED
//	xml:base %URI; #IMPLIED" >
#define STD_ATTRS\
	LPCTSTR id;\
	URI xml_base

//<!ENTITY % langSpaceAttrs
//	"xml:lang NMTOKEN #IMPLIED
//	xml:space (default|preserve) #IMPLIED" >
#define LANG_SPACE_ATTRS\
	LPCTSTR xml_lang;\
	BOOL xml_space

//<!ENTITY % testAttrs
//	"requiredFeatures %FeatureList; #IMPLIED
//	requiredExtensions %ExtensionList; #IMPLIED
//	systemLanguage %LanguageCodes; #IMPLIED" >

//<!ENTITY % PresentationAttributes-FillStroke
//	"fill %Paint; #IMPLIED
//	fill-opacity %OpacityValue; #IMPLIED
//	fill-rule %ClipFillRule; #IMPLIED
//	stroke %Paint; #IMPLIED
//	stroke-dasharray %StrokeDashArrayValue; #IMPLIED
//	stroke-dashoffset %StrokeDashOffsetValue; #IMPLIED
//	stroke-linecap (butt | round | square | inherit) #IMPLIED
//	stroke-linejoin (miter | round | bevel | inherit) #IMPLIED
//	stroke-miterlimit %StrokeMiterLimitValue; #IMPLIED
//	stroke-opacity %OpacityValue; #IMPLIED
//	stroke-width %StrokeWidthValue; #IMPLIED " >
#define PRESENTATION_ATTRIBUTES_FILL_STROKE\
	COLORREF fill;\
	OPACITY_VALUE fill_opacity;\
	FillMode fill_rule;\
	COLORREF stroke;\
	ARRAY stroke_dasharray;\
	LENGTH stroke_dashoffset;\
	LineCap stroke_linecap;\
	LineJoin stroke_linejoin;\
	NUMBER stroke_miterlimit;\
	OPACITY_VALUE stroke_opacity;\
	LENGTH stroke_width

//<!ENTITY % PresentationAttributes-Graphics
//	"clip-path %ClipPathValue; #IMPLIED
//	clip-rule %ClipFillRule; #IMPLIED
//	cursor %CursorValue; #IMPLIED
//	display (inline | block | list-item | run-in | compact | marker |
//	table | inline-table | table-row-group | table-header-group |
//	table-footer-group | table-row | table-column-group | table-column |
//	table-cell | table-caption | none | inherit) #IMPLIED
//	filter %FilterValue; #IMPLIED
//	image-rendering (auto | optimizeSpeed | optimizeQuality | inherit) #IMPLIED
//	mask %MaskValue; #IMPLIED
//	opacity %OpacityValue; #IMPLIED
//	pointer-events (visiblePainted | visibleFill | visibleStroke | visible |
//	painted | fill | stroke | all | none | inherit) #IMPLIED
//	shape-rendering (auto | optimizeSpeed | crispEdges | geometricPrecision | inherit) #IMPLIED
//	text-rendering (auto | optimizeSpeed | optimizeLegibility | geometricPrecision | inherit) #IMPLIED
//	visibility (visible | hidden | inherit) #IMPLIED " >
#define PRESENTATION_ATTRIBUTES_GRAPHICS\
	URI clip_path

#define SVG_RENDER_OBJECT_STRUCTURE\
	SvgTagType type;\
	DomNode* dom_node;\
	SvgRenderObject* parent;\
	SvgRenderObject* firstChild;\
	SvgRenderObject* lastChild;\
	SvgRenderObject* previousSibling;\
	SvgRenderObject* nextSibling;\
	/*presentation attributes*/\
	BOOL has_fill;\
	COLORREF fill;\
	FillMode fill_rule;\
	OPACITY_VALUE fill_opacity;\
	BOOL has_stroke;\
	COLORREF stroke;\
	NUMBER stroke_width;\
	LineCap stroke_linecap;\
	LineJoin stroke_linejoin;\
	OPACITY_VALUE stroke_opacity;\
	ARRAY stroke_dasharray;\
	NUMBER stroke_dashoffset;\
	NUMBER stroke_miterlimit;\
	NUMBER transform[9];\
	PRESENTATION_ATTRIBUTES_GRAPHICS
struct _SvgRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
};

/************************************************************************/
/*                               SVG                                    */
/************************************************************************/

struct _SvgSvgRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//xmlns CDATA #FIXED "http://www.w3.org/2000/svg"
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-All;
	BOOL has_view_box;
	ViewBox view_box;//viewBox %ViewBoxSpec; #IMPLIED
	PRESERVE_ASPECT_RATIO perserve_aspect_ratio;//preserveAspectRatio %PreserveAspectRatioSpec; 'xMidYMid meet'
	//zoomAndPan (disable | magnify) 'magnify'
	//%graphicsElementEvents;
	//%documentEvents;
	//version %Number; #FIXED "1.0"
	COORDINATE x;//x %Coordinate; #IMPLIED
	COORDINATE y;//y %Coordinate; #IMPLIED
	LENGTH width;//width %Length; #IMPLIED
	LENGTH height;//height %Length; #IMPLIED
	//	contentScriptType %ContentType; "text/ecmascript"
	//	contentStyleType %ContentType; "text/css" >
};

/************************************************************************/
/*                               GROUP                                  */
/************************************************************************/
struct _SvgGRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-All;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
};

/************************************************************************/
/*                           DEFINITIONS                                */
/************************************************************************/
struct _SvgDefsRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-All;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
};

/************************************************************************/
/*                             DESCIPTION                               */
/************************************************************************/
struct _SvgDescRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%langSpaceAttrs;
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%StructuredText;
};

/************************************************************************/
/*                              TITLE                                   */
/************************************************************************/
struct _SvgTitleRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%langSpaceAttrs;
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%StructuredText;
};

/************************************************************************/
/*                              PATH                                    */
/************************************************************************/
struct _SvgPathRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-Graphics;
	//%PresentationAttributes-Markers;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	//d %PathData; #REQUIRED
	//pathLength %Number; #IMPLIED
	ARRAY/*PathCommand*/ path_data;
};

/************************************************************************/
/*                              RECT                                    */
/************************************************************************/
struct _SvgRectRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-Graphics;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	COORDINATE x;//x %Coordinate; #IMPLIED
	COORDINATE y;//y %Coordinate; #IMPLIED
	LENGTH height;//width %Length; #REQUIRED
	LENGTH width;//height %Length; #REQUIRED
	LENGTH rx;//rx %Length; #IMPLIED
	LENGTH ry;//ry %Length; #IMPLIED
};

/************************************************************************/
/*                             CIRCLE                                   */
/************************************************************************/
struct _SvgCircleRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-Graphics;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	//cx %Coordinate; #IMPLIED
	//cy %Coordinate; #IMPLIED
	//r %Length; #REQUIRED
	NUMBER cx,cy,r;
};

/************************************************************************/
/*                            ELLIPSE                                   */
/************************************************************************/
struct _SvgEllipseRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-Graphics;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	//cx %Coordinate; #IMPLIED
	//cy %Coordinate; #IMPLIED
	//rx %Length; #REQUIRED
	//ry %Length; #REQUIRED
	NUMBER cx,cy,rx,ry;
};

/************************************************************************/
/*                             LINE                                     */
/************************************************************************/
struct _SvgLineRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-Graphics;
	//%PresentationAttributes-Markers;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	//x1 %Coordinate; #IMPLIED
	//y1 %Coordinate; #IMPLIED
	//x2 %Coordinate; #IMPLIED
	//y2 %Coordinate; #IMPLIED
	NUMBER x1,y1,x2,y2;
};

/************************************************************************/
/*                            POLYLINE                                  */
/************************************************************************/
struct _SvgPolylineRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-Graphics;
	//%PresentationAttributes-Markers;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	//points %Points; #REQUIRED
	ARRAY point_array;
};

/************************************************************************/
/*                            POLYGON                                   */
/************************************************************************/
struct _SvgPolygonRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-Graphics;
	//%PresentationAttributes-Markers;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	//points %Points; #REQUIRED
	ARRAY point_array;
};

/************************************************************************/
/*                              USE                                     */
/************************************************************************/
struct _SvgUseRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%xlinkRefAttrsEmbed;
	URI xlink_href;//xlink:href %URI; #REQUIRED
	//FIXME:xlink_href属性是required，但是有一些svg文件中的use的确没有xlink:href!!!
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-All;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	NUMBER x;//x %Coordinate; #IMPLIED
	NUMBER y;//y %Coordinate; #IMPLIED
	NUMBER width;//width %Length; #IMPLIED
	NUMBER height;//height %Length; #IMPLIED
};

/************************************************************************/
/*                            STYLE                                     */
/************************************************************************/
struct _SvgStyleRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//xml:space (preserve) #FIXED "preserve"
	//type %ContentType; #REQUIRED
	//media %MediaDesc; #IMPLIED
	//title %Text; #IMPLIED
};

/************************************************************************/
/*                            TEXT                                      */
/************************************************************************/
struct _SvgTextRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-FontSpecification;
	//%PresentationAttributes-Graphics;
	//%PresentationAttributes-TextContentElements;
	//%PresentationAttributes-TextElements;
	//transform %TransformList; #IMPLIED
	//%graphicsElementEvents;
	//x %Coordinates; #IMPLIED
	//y %Coordinates; #IMPLIED
	//dx %Lengths; #IMPLIED
	//dy %Lengths; #IMPLIED
	//rotate %Numbers; #IMPLIED
	//textLength %Length; #IMPLIED
	//lengthAdjust (spacing|spacingAndGlyphs) #IMPLIED
};

/************************************************************************/
/*                          CLIP PATH                                   */
/************************************************************************/
struct _SvgClipPathRenderObject{
	SVG_RENDER_OBJECT_STRUCTURE;
	//%stdAttrs;
	//%testAttrs;
	//%langSpaceAttrs;
	//externalResourcesRequired %Boolean; #IMPLIED
	//class %ClassList; #IMPLIED
	//style %StyleSheet; #IMPLIED
	//%PresentationAttributes-Color;
	//%PresentationAttributes-FillStroke;
	//%PresentationAttributes-FontSpecification;
	//%PresentationAttributes-Graphics;
	//%PresentationAttributes-TextContentElements;
	//%PresentationAttributes-TextElements;
	//transform %TransformList; #IMPLIED
	//clipPathUnits (userSpaceOnUse | objectBoundingBox) #IMPLIED
};

/************************************************************************/
/*                             私有函数声明                             */
/************************************************************************/
PRIVATE PathCommand* path_command_create();
PRIVATE void path_command_destroy(PathCommand* thiz);
PRIVATE void path_command_copy(PathCommand* dst, PathCommand* src);
PRIVATE BOOL get_standard_attribute(SvgRenderObject* thiz,ATTR* attr);
PRIVATE void svg_render_object_initialize(SvgRenderObject* thiz);
//              |0 1 2|
//矩阵排列形式为|3 4 5|
//              |6 7 8|
PRIVATE void string_to_matrix(LPTSTR str,NUMBER *m_last);
//解析PathData信息，将其由字符串转化为链表
PRIVATE DList* string_to_path_data_list(LPTSTR str);
PRIVATE ARRAY string_to_point_array(LPTSTR str);
PRIVATE void string_to_fill_rull(LPCTSTR str, FillMode* fill_rule);
PRIVATE void string_to_linecap(LPCTSTR str, LineCap* line_cap);
PRIVATE void string_to_linejoin(LPCTSTR str, LineJoin* line_join);
PRIVATE void svg_render_object_inherit_attributes_from_parent(SvgRenderObject* thiz, SvgRenderObject* parent);
PRIVATE void parse_style_attribute(SvgRenderObject* thiz, LPCTSTR str);
PRIVATE ViewBox string_to_view_box(LPTSTR str);
PRIVATE ARRAY string_to_dasharray(LPTSTR str);
PRIVATE void string_to_preserve_aspect_ratio(LPTSTR str, PRESERVE_ASPECT_RATIO* par);

/************************************************************************/
/*                             公有函数实现                             */
/************************************************************************/
PUBLIC void svg_render_object_append_child(SvgRenderObject* thiz, SvgRenderObject* child){
	return_if_fail(thiz && child);

	if(thiz->lastChild){
		thiz->lastChild->nextSibling = child;
		child->previousSibling = thiz->lastChild;
		thiz->lastChild = child;
	}else{
		thiz->firstChild = thiz->lastChild = child;
	}
	child->parent = thiz;
}

PUBLIC SvgRenderObject* svg_render_object_get_first_child(SvgRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->firstChild;
}

PUBLIC SvgRenderObject* svg_render_object_get_next_sibling(SvgRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->nextSibling;
}

PUBLIC BOOL svg_render_object_has_fill(SvgRenderObject* thiz){
	return_val_if_fail(thiz, TRUE);//has_fill默认值为TRUE
	return thiz->has_fill;
}

PUBLIC COLORREF svg_render_object_get_fill_color(SvgRenderObject* thiz){
	return_val_if_fail(thiz, RGB(0, 0, 0));
	return thiz->fill;
}

PUBLIC NUMBER svg_render_object_get_fill_opacity(SvgRenderObject* thiz){
	return_val_if_fail(thiz, 1);
	return thiz->fill_opacity;
}

PUBLIC FillMode svg_render_object_get_fill_rule(SvgRenderObject* thiz){
	return_val_if_fail(thiz, FillModeWinding);
	return thiz->fill_rule;
}

PUBLIC BOOL svg_render_object_has_stroke(SvgRenderObject* thiz){
	return_val_if_fail(thiz, FALSE);//has_stroke默认值为FALSE
	return thiz->has_stroke;
}

PUBLIC NUMBER svg_render_object_get_stroke_width(SvgRenderObject* thiz){
	return_val_if_fail(thiz, 0.0);
	return thiz->stroke_width;
}

PUBLIC COLORREF svg_render_object_get_stroke_color(SvgRenderObject* thiz){
	return_val_if_fail(thiz, RGB(0, 0, 0));
	return thiz->stroke;
}

PUBLIC LineCap svg_render_object_get_stroke_linecap(SvgRenderObject* thiz){
	return_val_if_fail(thiz, LineCapFlat);
	return thiz->stroke_linecap;
}

PUBLIC LineJoin svg_render_object_get_stroke_linejoin(SvgRenderObject* thiz){
	return_val_if_fail(thiz, LineJoinMiter);
	return thiz->stroke_linejoin;
}

PUBLIC NUMBER svg_render_object_get_stroke_opacity(SvgRenderObject* thiz){
	return_val_if_fail(thiz, 1);
	return thiz->stroke_opacity;
}

PUBLIC ARRAY svg_render_object_get_stroke_dasharray(SvgRenderObject* thiz){
	return_val_if_fail(thiz, array_empty);
	return thiz->stroke_dasharray;
}

PUBLIC NUMBER svg_render_object_get_stroke_dashoffset(SvgRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->stroke_dashoffset;
}

PUBLIC NUMBER svg_render_object_get_stroke_miterlimit(SvgRenderObject* thiz){
	return_val_if_fail(thiz, 4);
	return thiz->stroke_miterlimit;
}

PUBLIC NUMBER* svg_render_object_get_transform(SvgRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->transform;
}

PUBLIC LPCTSTR svg_render_object_get_clip_path_name(SvgRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->clip_path.name;
}

PUBLIC SvgClipPathRenderObject* svg_render_object_get_clip_path_element(SvgRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return (SvgClipPathRenderObject*)thiz->clip_path.target;
}

PUBLIC void svg_render_object_set_clip_path_element(SvgRenderObject* thiz, SvgClipPathRenderObject* element){
	return_if_fail(thiz && element);
	thiz->clip_path.target = element;
}

PUBLIC SvgRenderTree* svg_render_tree_create(){
	SvgRenderTree* thiz = (SvgRenderTree*)calloc(1, sizeof(SvgRenderTree));
	thiz->root = NULL;
	return thiz;
}

PUBLIC void svg_render_tree_destroy(SvgRenderTree* thiz){
	return_if_fail(thiz);
	free(thiz);
}

PUBLIC SvgRenderObject* svg_render_tree_get_root(SvgRenderTree* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->root;
}

PUBLIC void svg_render_tree_set_root(SvgRenderTree* thiz, SvgRenderObject* root){
	return_if_fail(thiz && root);
	thiz->root = root;
}

PUBLIC SvgTagType string_to_svg_render_object_type(LPCTSTR str){
	int index;
	for (index = sizeof SvgRenderObjectTypes / sizeof SvgRenderObjectTypes[0] - 1; index > 0; index--)
		if (!_tcscmp(str, SvgRenderObjectTypes[index].mnemonic))
			break;
	return (SvgTagType)SvgRenderObjectTypes[index].token;
}

PUBLIC SvgRenderObject* svg_render_object_factory_create_render_object(DomNode* dom_node, SvgRenderObject* parent){
	void* thiz = NULL;
	SvgTagType type;
	return_val_if_fail(dom_node, NULL);
	type = string_to_svg_render_object_type(dom_node_get_node_name(dom_node));
	switch(type){
	case SVG_TAG_TYPE_PATH: thiz = svg_path_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_RECT: thiz = svg_rect_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_CIRCLE: thiz = svg_circle_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_ELLIPSE: thiz = svg_ellipse_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_LINE: thiz = svg_line_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_POLYLINE: thiz = svg_polyline_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_POLYGON: thiz = svg_polygon_render_object_create(dom_node, parent); break;

	case SVG_TAG_TYPE_SVG: thiz = svg_svg_render_object_create(dom_node); break;
	case SVG_TAG_TYPE_G: thiz = svg_g_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_USE: thiz = svg_use_render_object_create(dom_node, parent); break;
	case SVG_TAG_TYPE_TITLE: break;
	case SVG_TAG_TYPE_DESC: break;
	case SVG_TAG_TYPE_DEFS: break;

	case SVG_TAG_TYPE_CLIP_PATH: break;
	}
	return (SvgRenderObject*)thiz;
}

PUBLIC SvgTagType svg_render_object_get_type(SvgRenderObject* thiz){
	return_val_if_fail(thiz, SVG_TAG_TYPE_UNKNOWN);
	return thiz->type;
}

PUBLIC SvgRenderObject* svg_render_object_get_parent(SvgRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->parent;
}

PRIVATE LPCTSTR parse_url(LPCTSTR str){
	int i = 0;
	if(!_tcsnicmp(str, _T("url("), 4)){//NOTE: case insensitive
		str += 4;
		while(str[i] && str[i] != _T(')')) i++;
		if(str[i] == _T(')')){
			return get_substring(str, 0, i);
		}
	}
	return NULL;
}

PUBLIC SvgPathRenderObject* svg_path_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgPathRenderObject* thiz = (SvgPathRenderObject*)calloc(1, sizeof(SvgPathRenderObject));
	Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));//FIXME：应该对dom_node的attrs为NULL的情况进行处理
	ATTR* attr = NULL;
	//svg_render_object_initialize((SvgRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_PATH;
	thiz->dom_node = dom_node;//FIXME：这条语句应该是统一处理的
	while(attr = (ATTR*)iterator_get(attr_iter)){
		attr_iter->next(attr_iter);
		if(get_standard_attribute((SvgRenderObject*)thiz, attr))continue;
		if(!_tcscmp(attr->name, _T("d"))){//Path Data
			DList* path_data_list = string_to_path_data_list(attr->value);
			thiz->path_data = dlist_to_array_deep(path_data_list, sizeof(PathCommand), (DEEP_COPY)path_command_copy);
			dlist_destroy(path_data_list);
		}else if(!_tcscmp(attr->name, _T("clip-path"))){//Clip Path
			thiz->clip_path.name = parse_url(attr->value);
		}
	}
	attr_iter->destroy(attr_iter);	//测试代码
	return thiz;
}

PUBLIC SvgRectRenderObject* svg_rect_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgRectRenderObject* thiz = (SvgRectRenderObject*)calloc(1, sizeof(SvgRectRenderObject));
	Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
	ATTR* attr;
	BOOL rx_is_set = FALSE, ry_is_set = FALSE;
	//svg_render_object_initialize((SvgRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_RECT;
	thiz->dom_node = dom_node;
	while(attr = (ATTR*)iterator_get(attr_iter)){
		attr_iter->next(attr_iter);
		if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
		if(!_tcscmp(attr->name,_T("x"))){
			thiz->x = string_to_coordinate(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("y"))){
			thiz->y = string_to_coordinate(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("width"))){
			thiz->width = string_to_length(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("height"))){
			thiz->height = string_to_length(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("rx"))){
			rx_is_set = TRUE;
			thiz->rx = string_to_length(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("ry"))){
			ry_is_set = TRUE;
			thiz->ry = string_to_length(attr->value, NULL);
		}
	}
	attr_iter->destroy(attr_iter);
	//Adjust values
	if(rx_is_set || ry_is_set){
		//If only one of rx and ry is set, set the same value to the unset one
		if(rx_is_set && !ry_is_set){
			thiz->ry = thiz->rx;
		}else if(!rx_is_set && ry_is_set){
			thiz->rx = thiz->ry;
		}
		/*	NOTE: 
		 *	Should not compare width and rx here(height and ry likewise), 
		 *	because percentage value cannot be determined at this time.
		 */
	}
	return thiz;
}

PUBLIC SvgCircleRenderObject* svg_circle_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgCircleRenderObject* thiz = (SvgCircleRenderObject*)calloc(1, sizeof(SvgCircleRenderObject));
	Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
	ATTR* attr;
	//svg_render_object_initialize((SvgRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_CIRCLE;
	thiz->dom_node = dom_node;
	while(attr = (ATTR*)iterator_get(attr_iter)){
		attr_iter->next(attr_iter);
		if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
		if(!_tcscmp(attr->name,_T("cx"))){
			thiz->cx = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("cy"))){
			thiz->cy = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("r"))){
			thiz->r = string_to_number(attr->value, NULL);
		}
	}
	attr_iter->destroy(attr_iter);
	return thiz;
}

PUBLIC SvgEllipseRenderObject* svg_ellipse_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgEllipseRenderObject* thiz = (SvgEllipseRenderObject*)calloc(1, sizeof(SvgEllipseRenderObject));
	Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
	ATTR* attr;
	//svg_render_object_initialize((SvgRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_ELLIPSE;
	thiz->dom_node = dom_node;
	while(attr = (ATTR*)iterator_get(attr_iter)){
		attr_iter->next(attr_iter);
		if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
		if(!_tcscmp(attr->name,_T("cx"))){
			thiz->cx = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("cy"))){
			thiz->cy = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("rx"))){
			thiz->rx = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("ry"))){
			thiz->ry = string_to_number(attr->value, NULL);
		}
	}
	attr_iter->destroy(attr_iter);
	return thiz;
}

PUBLIC SvgLineRenderObject* svg_line_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgLineRenderObject* thiz = (SvgLineRenderObject*)calloc(1, sizeof(SvgLineRenderObject));
	Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
	ATTR* attr;
	//svg_render_object_initialize((SvgRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_LINE;
	thiz->dom_node = dom_node;
	while(attr = (ATTR*)iterator_get(attr_iter)){
		attr_iter->next(attr_iter);
		if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
		if(!_tcscmp(attr->name,_T("x1"))){
			thiz->x1 = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("y1"))){
			thiz->y1 = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("x2"))){
			thiz->x2 = string_to_number(attr->value, NULL);
		}else if(!_tcscmp(attr->name,_T("y2"))){
			thiz->y2 = string_to_number(attr->value, NULL);
		}
	}
	attr_iter->destroy(attr_iter);
	return thiz;
}

PUBLIC SvgPolylineRenderObject* svg_polyline_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgPolylineRenderObject* thiz = (SvgPolylineRenderObject*)calloc(1, sizeof(SvgPolylineRenderObject));
	Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
	ATTR* attr;
	//svg_render_object_initialize((SvgRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_POLYLINE;
	thiz->dom_node = dom_node;
	while(attr = (ATTR*)iterator_get(attr_iter)){
		attr_iter->next(attr_iter);
		if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
		if(!_tcscmp(attr->name,_T("points"))){
			thiz->point_array = string_to_point_array(attr->value);
		}
	}
	attr_iter->destroy(attr_iter);
	return thiz;
}

PUBLIC SvgPolygonRenderObject* svg_polygon_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgPolygonRenderObject* thiz = (SvgPolygonRenderObject*)calloc(1, sizeof(SvgPolygonRenderObject));
	Iterator* attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
	ATTR* attr;
	//svg_render_object_initialize((SvgRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_POLYGON;
	thiz->dom_node = dom_node;
	while(attr = (ATTR*)iterator_get(attr_iter)){
		attr_iter->next(attr_iter);
		if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
		if(!_tcscmp(attr->name,_T("points"))){
			thiz->point_array = string_to_point_array(attr->value);
		}
	}
	attr_iter->destroy(attr_iter);
	return thiz;
}

PUBLIC SvgSvgRenderObject* svg_svg_render_object_create(DomNode* dom_node){
	SvgSvgRenderObject* thiz = (SvgSvgRenderObject*)calloc(1, sizeof(SvgSvgRenderObject));
	Iterator* attr_iter = NULL;
	DList* attr_list = NULL;
	ATTR* attr = NULL;
	svg_render_object_initialize((SvgRenderObject*)thiz);
	//svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_SVG;
	attr_list = dom_node_get_attributes(dom_node);
	thiz->dom_node = dom_node;

	thiz->width = thiz->height = length_100_percent;
	thiz->perserve_aspect_ratio.align = PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMID;
	thiz->perserve_aspect_ratio.meet_or_slice = PRESERVE_ASPECT_RATIO_MORS_MEET;

	if(attr_list){
		attr_iter = dlist_iterator_create(attr_list);
		while(attr = (ATTR*)iterator_get(attr_iter)){
			attr_iter->next(attr_iter);
			if(get_standard_attribute((SvgRenderObject*)thiz, attr))continue;
			if(!_tcscmp(attr->name, _T("x"))){//x
				thiz->x = string_to_length(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("y"))){//y
				thiz->y = string_to_length(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("width"))){//width
				thiz->width = string_to_length(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("height"))){//height
				thiz->height = string_to_length(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("viewBox"))){//viewBox
				thiz->has_view_box = TRUE;
				thiz->view_box = string_to_view_box(attr->value);
			}else if(!_tcscmp(attr->name, _T("preserveAspectRatio"))){//preserveAspectRatio
				string_to_preserve_aspect_ratio(attr->value, &thiz->perserve_aspect_ratio);
			}
		}
		attr_iter->destroy(attr_iter);
	}
	return thiz;
}

PUBLIC SvgGRenderObject* svg_g_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgGRenderObject* thiz = (SvgGRenderObject*)calloc(1, sizeof(SvgGRenderObject));
	Iterator* attr_iter = NULL;
	DList* attr_list = NULL;
	ATTR* attr = NULL;
	//svg_render_object_initialize((SvgGRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_G;
	thiz->dom_node = dom_node;
	attr_list = dom_node_get_attributes(dom_node);
	if(attr_list){
		attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
		while(attr = (ATTR*)iterator_get(attr_iter)){
			attr_iter->next(attr_iter);
			if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
		}
		attr_iter->destroy(attr_iter);
	}
	return thiz;
}

PUBLIC SvgUseRenderObject* svg_use_render_object_create(DomNode* dom_node, SvgRenderObject* parent){
	SvgUseRenderObject* thiz = (SvgUseRenderObject*)calloc(1, sizeof(SvgUseRenderObject));
	Iterator* attr_iter = NULL;
	DList* attr_list = NULL;
	ATTR* attr = NULL;
	//svg_render_object_initialize((SvgGRenderObject*)thiz);
	svg_render_object_inherit_attributes_from_parent((SvgRenderObject*)thiz, parent);
	thiz->type = SVG_TAG_TYPE_USE;
	thiz->dom_node = dom_node;
	attr_list = dom_node_get_attributes(dom_node);
	if(attr_list){
		attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
		while(attr = (ATTR*)iterator_get(attr_iter)){
			attr_iter->next(attr_iter);
			if(get_standard_attribute((SvgRenderObject*)thiz,attr))continue;
			if(!_tcscmp(attr->name, _T("x"))){//x
				thiz->x = string_to_number(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("y"))){//y
				thiz->y = string_to_number(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("width"))){//width
				thiz->width = string_to_number(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("height"))){//height
				thiz->height = string_to_number(attr->value, NULL);
			}else if(!_tcscmp(attr->name, _T("xlink:href"))){//xlink:href
				thiz->xlink_href.name = copy_string(attr->value);
			}
		}
		attr_iter->destroy(attr_iter);
	}
	return thiz;
}

PUBLIC SvgClipPathRenderObject* svg_clip_path_render_object_create(DomNode* dom_node){
	SvgClipPathRenderObject* thiz = (SvgClipPathRenderObject*)calloc(1, sizeof(SvgClipPathRenderObject));
	Iterator* attr_iter = NULL;
	DList* attr_list = NULL;
	ATTR* attr = NULL;
	svg_render_object_initialize((SvgRenderObject*)thiz);
	thiz->type = SVG_TAG_TYPE_CLIP_PATH;
	thiz->dom_node = dom_node;
	attr_list = dom_node_get_attributes(dom_node);
	if(attr_list){
		attr_iter = dlist_iterator_create(dom_node_get_attributes(dom_node));
		while(attr = (ATTR*)iterator_get(attr_iter)){
			attr_iter->next(attr_iter);
			if(get_standard_attribute((SvgRenderObject*)thiz,attr)) continue;
			//FIXME:clipPathUnits
		}
		attr_iter->destroy(attr_iter);
	}
	return thiz;
}

PUBLIC ViewBox svg_svg_render_object_get_view_box(SvgSvgRenderObject* thiz){
	ViewBox empty_view_box = {0};
	return_val_if_fail(thiz, empty_view_box);
	return thiz->view_box;
}

PUBLIC BOOL svg_svg_render_object_has_view_box(SvgSvgRenderObject* thiz){
	return_val_if_fail(thiz, FALSE);
	return thiz->has_view_box;
}


PUBLIC LENGTH svg_svg_render_object_get_width(SvgSvgRenderObject* thiz){
	return_val_if_fail(thiz, length_100_percent);
	return thiz->width;
}

PUBLIC LENGTH svg_svg_render_object_get_height(SvgSvgRenderObject* thiz){
	return_val_if_fail(thiz, length_100_percent);
	return thiz->height;
}

PUBLIC PRESERVE_ASPECT_RATIO svg_svg_render_object_get_preserve_aspect_ratio(SvgSvgRenderObject* thiz){
	//FIXME: return_val_if_fail
	return thiz->perserve_aspect_ratio;
}

PUBLIC ARRAY svg_path_render_object_get_path_data(SvgPathRenderObject* thiz){
	return_val_if_fail(thiz, array_empty);
	return thiz->path_data;
}

PUBLIC COORDINATE svg_rect_render_object_get_x(SvgRectRenderObject* thiz){
	return_val_if_fail(thiz, coordinate_zero);
	return thiz->x;
}

PUBLIC COORDINATE svg_rect_render_object_get_y(SvgRectRenderObject* thiz){
	return_val_if_fail(thiz, coordinate_zero);
	return thiz->y;
}

PUBLIC LENGTH svg_rect_render_object_get_width(SvgRectRenderObject* thiz){
	return_val_if_fail(thiz, length_zero);
	return thiz->width;
}

PUBLIC LENGTH svg_rect_render_object_get_height(SvgRectRenderObject* thiz){
	return_val_if_fail(thiz, length_zero);
	return thiz->height;
}

PUBLIC LENGTH svg_rect_render_object_get_rx(SvgRectRenderObject* thiz){
	return_val_if_fail(thiz, length_zero);
	return thiz->rx;
}

PUBLIC LENGTH svg_rect_render_object_get_ry(SvgRectRenderObject* thiz){
	return_val_if_fail(thiz, length_zero);
	return thiz->ry;
}

PUBLIC NUMBER svg_circle_render_object_get_center_x(SvgCircleRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->cx;
}

PUBLIC NUMBER svg_circle_render_object_get_center_y(SvgCircleRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->cy;
}

PUBLIC NUMBER svg_circle_render_object_get_radius(SvgCircleRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->r;
}

PUBLIC NUMBER svg_ellipse_render_object_get_center_x(SvgEllipseRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->cx;
}

PUBLIC NUMBER svg_ellipse_render_object_get_center_y(SvgEllipseRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->cy;
}

PUBLIC NUMBER svg_ellipse_render_object_get_radius_x(SvgEllipseRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->rx;
}

PUBLIC NUMBER svg_ellipse_render_object_get_radius_y(SvgEllipseRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->ry;
}

PUBLIC NUMBER svg_line_render_object_get_x1(SvgLineRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->x1;
}

PUBLIC NUMBER svg_line_render_object_get_y1(SvgLineRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->y1;
}

PUBLIC NUMBER svg_line_render_object_get_x2(SvgLineRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->x2;
}

PUBLIC NUMBER svg_line_render_object_get_y2(SvgLineRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->y2;
}

PUBLIC ARRAY svg_polyline_render_object_get_point_array(SvgPolylineRenderObject* thiz){
	return_val_if_fail(thiz, array_empty);
	return thiz->point_array;
}

PUBLIC ARRAY svg_polygon_render_object_get_point_array(SvgPolygonRenderObject* thiz){
	return_val_if_fail(thiz, array_empty);
	return thiz->point_array;
}

PUBLIC LPCTSTR svg_use_render_object_get_xlink_href_name(SvgUseRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return thiz->xlink_href.name;
}

PUBLIC DomElement* svg_use_render_object_get_xlink_href_element(SvgUseRenderObject* thiz){
	return_val_if_fail(thiz, NULL);
	return (DomElement*)thiz->xlink_href.target;
}

PUBLIC void svg_use_render_object_set_xlink_href_element(SvgUseRenderObject* thiz, DomElement* dom_element){
	return_if_fail(thiz && dom_element);
	thiz->xlink_href.target = dom_element;
}

PUBLIC NUMBER svg_use_render_object_get_x(SvgUseRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->x;
}

PUBLIC NUMBER svg_use_render_object_get_y(SvgUseRenderObject* thiz){
	return_val_if_fail(thiz, 0);
	return thiz->y;
}

/************************************************************************/
/*                             私有函数实现                             */
/************************************************************************/
PRIVATE PathCommand* path_command_create(){
	PathCommand* thiz = (PathCommand*)calloc(1, sizeof(PathCommand));
	return thiz;
}

PRIVATE void path_command_destroy(PathCommand* thiz){
	free(thiz->data_array.data);
	free(thiz);
}

PRIVATE void path_command_copy(PathCommand* dst, PathCommand* src){
	dst->cmd_type = src->cmd_type;
	dst->data_array.length = src->data_array.length;
	dst->data_array.data = malloc(sizeof(NUMBER) * dst->data_array.length);
	memcpy(dst->data_array.data, src->data_array.data, sizeof(NUMBER) * dst->data_array.length);
}

PRIVATE BOOL get_standard_attribute(SvgRenderObject* thiz, ATTR* attr){
	if(!_tcscmp(attr->name, _T("fill"))){//fill
		thiz->has_fill = _tcscmp(attr->value,_T("none"));
		if(thiz->has_fill)
			thiz->fill = string_to_color(attr->value);
	}else if(!_tcscmp(attr->name,_T("fill-rule"))){//fill-rull
		string_to_fill_rull(attr->value, &thiz->fill_rule);
	}else if(!_tcscmp(attr->name, _T("fill-opacity"))){//fill-opacity
		thiz->fill_opacity = string_to_number(attr->value, NULL);
	}else if(!_tcscmp(attr->name, _T("stroke"))){//stroke
		thiz->has_stroke = _tcscmp(attr->value, _T("none"));
		if(thiz->has_stroke)
			thiz->stroke = string_to_color(attr->value);
	}else if(!_tcscmp(attr->name, _T("stroke-width"))){//stroke-width
		thiz->stroke_width = string_to_number(attr->value, NULL);
	}else if(!_tcscmp(attr->name, _T("stroke-linecap"))){//stroke-linecap
		string_to_linecap(attr->value, &thiz->stroke_linecap);
	}else if(!_tcscmp(attr->name, _T("stroke-linejoin"))){//stroke-linejoin
		string_to_linejoin(attr->value, &thiz->stroke_linejoin);
	}else if(!_tcscmp(attr->name, _T("stroke-opacity"))){//stroke-opacity
		thiz->stroke_opacity = string_to_number(attr->value, NULL);
	}else if(!_tcscmp(attr->name, _T("stroke-dasharray"))){//stroke-dasharray
		thiz->stroke_dasharray = string_to_dasharray(attr->value);
	}else if(!_tcscmp(attr->name, _T("stroke-dashoffset"))){//stroke-dashoffset
		thiz->stroke_dashoffset = string_to_number(attr->value, NULL);
	}else if(!_tcscmp(attr->name, _T("stroke-miterlimit"))){//stroke-miterlimit
		thiz->stroke_miterlimit = string_to_number(attr->value, NULL);
	}else if(!_tcscmp(attr->name, _T("transform"))){//transform
		string_to_matrix(attr->value, thiz->transform);
	}else if(!_tcscmp(attr->name, _T("style"))){//style
		parse_style_attribute(thiz, attr->value);
	}else {
		return FALSE;
	}
	return TRUE;
}

PRIVATE void svg_render_object_initialize(SvgRenderObject* thiz){
	thiz->has_fill = TRUE;//若不设置fill，则填充黑色
	thiz->fill = RGB(0, 0, 0);//fill初始值为black
	thiz->fill_opacity = 1.0;//0.0代表透明，1.0代表不透明
	thiz->fill_rule = FillModeWinding;//fill-rule初始值为nonzero

	thiz->has_stroke = FALSE;//stroke初始值为none
	thiz->stroke = RGB(0, 0, 0);
	thiz->stroke_width = 1.0;//stroke-width初始值为1
	thiz->stroke_linecap = LineCapFlat;//stroke-linecap的初始值为butt
	thiz->stroke_linejoin = LineJoinMiter;//stroke-linejoin的初始值为miter
	thiz->stroke_opacity = 1.0;//stroke-opacity的初始值为1
	thiz->stroke_dasharray.data = NULL;//stroke-dasharray的初始值为none
	thiz->stroke_dasharray.length = 0;
	thiz->stroke_miterlimit = 4.0;//stroke-miterlimit的初始值为4
	thiz->stroke_dashoffset = 0;//stroke-dashoffset的初始值为0

	memcpy(thiz->transform, &unit_matrix, sizeof(NUMBER)*9); //单位矩阵
}

//              |0 1 2|
//矩阵排列形式为|3 4 5|
//              |6 7 8|
PRIVATE void string_to_matrix(LPTSTR str, NUMBER *m_last){
	NUMBER m_in[9]={0}, m_rst[9]={0};
	LPCTSTR start;
	memcpy(m_last, unit_matrix, sizeof(NUMBER) * 9);
	//这里考虑换一种形式，先将变换名保存下来，str指到(内部，然后_tcsncmp根据不同的变换取字符
	while(*str){
		memcpy(m_in, unit_matrix, sizeof(NUMBER) * 9);//置m_in为单位矩阵
		OMIT_WSP(str);
		start = str;
		while(*str && *str!='(') str++;//滤过变换名
		if(*str!='(')break;//非正常退出
		str++;
		OMIT_WSP(str);//滤过(后的空格

		if(!_tcsncmp(start, _T("matrix"), 6)){
			//matrix(<a> <b> <c> <d> <e> <f>)
			//6个数值
			//     |a c e|
			//tm = |b d f|
			//     |0 0 1|
			m_in[0] = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			m_in[3] = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			m_in[1] = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			m_in[4] = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			m_in[2] = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			m_in[5] = string_to_number(str, &str);
		}else if(!_tcsncmp(start, _T("translate"), 9)){
			//translate(<tx> [<ty>])
			//若未出现ty，则记ty=0
			//1个或2个数值
			//     |1 0 tx|
			//tm = |0 1 ty|
			//     |0 0  1|
			m_in[2] = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			if(*str == ')'){
				m_in[5] = 0;
			}else{
				m_in[5] = string_to_number(str, &str);
			}
		}else if(!_tcsncmp(start, _T("scale"), 5)){
			//scale(<sx> [<sy>])
			//若未出现sy，则记sy=sx
			//1个或2个数值
			//     |sx  0  0|
			//tm = |0  sy  0|
			//     |0   0  1|
			m_in[0] = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			if(*str == ')'){
				m_in[4] = m_in[0];
			}else{
				m_in[4] = string_to_number(str, &str);
			}
		}else if(!_tcsncmp(start, _T("rotate"), 6)){
			//rotate(<rotate-angle> [<cx> <cy>])
			//若未出现cx、cy，则记cx=cy=0
			//1个或3个数值
			//     |cos(a) -sin(a)   -cx*cos(a)+cy*sin(a)+cx|
			//tm = |sin(a)  cos(a)   -cx*sin(a)-cy*cos(a)+cy|
			//     |0           0              1            |
			NUMBER angle = 0.0f, cos_val = 1.0f, sin_val = 0.0f, cx = 0.0f, cy = 0.0f;
			angle = string_to_number(str, &str) / DEGREE;
			OMIT_COMMA_WSP(str);
			cx = string_to_number(str, &str);
			OMIT_COMMA_WSP(str);
			cy = string_to_number(str, &str);
			cos_val = cosf(angle);
			sin_val = sinf(angle);
			m_in[0] = cos_val;
			m_in[1] = -sin_val;
			m_in[2] = -cx * cos_val + cy * sin_val + cx;
			m_in[3] = sin_val;
			m_in[4] = cos_val;
			m_in[5] = -cx * sin_val - cy * cos_val + cy;
		}else if(!_tcsncmp(start, _T("skewX"), 5)){
			//skewX(<skew-angle>)
			//1个数值
			//     |1 tan(a) 0|
			//tm = |0    1   0|
			//     |0    0   1|
			NUMBER angle = 0, tan_val = 0;
			angle = string_to_number(str, &str) / DEGREE;
			tan_val = tanf(angle);
			m_in[1] = tan_val;
		}else if(!_tcsncmp(start, _T("skewY"), 5)){
			//skewY(<skew-angle>)
			//1个数值
			//     |1      0 0|
			//tm = |tan(a) 1 0|
			//     |0      0 1|
			NUMBER angle = 0, tan_val = 0;
			angle = string_to_number(str, &str) / DEGREE;
			tan_val = tanf(angle);
			m_in[3] = tan_val;
		}
		multiply_matrix(m_last, m_in, m_rst, 3, 3, 3);
		memcpy(m_last, m_rst, sizeof(m_rst));//将m_rst矩阵赋给m_last
		while(*str && *str != ')')str++;
		if(!*str) break;
		str++;
	}
}

//解析PathData信息，将其由字符串转化为PathCommand的数组
PRIVATE DList* string_to_path_data_list(LPTSTR str){
	DList* path_data_list;
	DList* data_list;
	PathCommand* path_cmd;
	NUMBER *data;
	return_val_if_fail(str, NULL);
	path_data_list = dlist_create((DListDataDestroyFunc)path_command_destroy);
	OMIT_WSP(str);
	while(*str){
		path_cmd = path_command_create();
		path_cmd->cmd_type = *str;
		str++;
		OMIT_WSP(str);
		data_list = dlist_create(free);
		do{
			data = (NUMBER*)malloc(sizeof(NUMBER));
			*data = string_to_number(str, &str);
			dlist_append(data_list, data);
			OMIT_COMMA_WSP(str);//FIXME:事实上，真实的svg并没有那么严谨，点之间也可以用逗号分隔。有时间对逗号进行细致讨论。
		}while((*str >= '0' && *str <= '9') || *str == _T('-') || *str == _T('.'));
		path_cmd->data_array = dlist_to_array_shallow(data_list, sizeof(NUMBER));
		dlist_destroy(data_list);
		dlist_append(path_data_list, path_cmd);
		OMIT_WSP(str);
	}
	return path_data_list;
}

PRIVATE ARRAY string_to_point_array(LPTSTR str){
	ARRAY point_array = {0};
	LPTSTR start = NULL;
	REAL_POINT* point;
	OMIT_WSP(str);
	start = str;
	//FIXME:以下循环需要优化，如果points中有无法转化为REAL的字符，那么需要特殊处理
	//FIXME:point数字可能有单位！以下代码无法处理！
	while(*str){
		OMIT_NUMBER(str);
		OMIT_COMMA_WSP(str);
		OMIT_NUMBER(str);
		OMIT_COMMA_WSP(str);//FIXME:有时间对points中的逗号进行细致讨论
		point_array.length++;
	}

	point_array.data = malloc(point_array.length * sizeof(REAL_POINT));
	point = (REAL_POINT*)point_array.data;
	str = start;
	while(*str){
		point->x = string_to_number(str, &str);
		OMIT_COMMA_WSP(str);
		point->y = string_to_number(str, &str);
		OMIT_COMMA_WSP(str);
		point++;
	}
	return point_array;
}

PRIVATE void string_to_fill_rull(LPCTSTR str, FillMode* fill_rule){
	int index;
	//Fill rule is case-insensitive, and 'nonzero' is the default fill rule.
	if(!_tcsicmp(str, _T("inherit"))){
		return;
	}
	for (index = sizeof FillRull / sizeof FillRull[0] - 1; index > 0; index--)
		if (!_tcsicmp(str, FillRull[index].mnemonic))
			break;
	*fill_rule = (FillMode)FillRull[index].token;
}

PRIVATE void string_to_linecap(LPCTSTR str, LineCap* line_cap){
	int index;
	//Line cap is case-insensitive, and 'butt' is the default line cap.
	if(!_tcsicmp(str, _T("inherit"))){
		return;
	}
	//FIXME:左右有空格的情况尚未考虑
	for (index = sizeof StrokeLineCaps / sizeof StrokeLineCaps[0] - 1; index > 0; index--)
		if (!_tcsicmp(str, StrokeLineCaps[index].mnemonic))
			break;
	*line_cap = (LineCap)StrokeLineCaps[index].token;
}

PRIVATE void string_to_linejoin(LPCTSTR str, LineJoin* line_join){
	int index;
	//Line join is case-insensitive, and 'miter' is the default line join.
	if(!_tcsicmp(str, _T("inherit"))){
		return;
	}
	for (index = sizeof StrokeLineJoins / sizeof StrokeLineJoins[0] - 1; index > 0; index--)
		if (!_tcsicmp(str, StrokeLineJoins[index].mnemonic))
			break;
	*line_join = (LineJoin)StrokeLineJoins[index].token;
}

#define COPY_NUM_ARRAY(dst, src)\
	do{\
		dst.length = src.length;\
		dst.data = malloc(sizeof(NUMBER) * src.length);\
		memcpy(dst.data, src.data, sizeof(NUMBER) * src.length);\
	}while(0)
PRIVATE void svg_render_object_inherit_attributes_from_parent(SvgRenderObject* thiz, SvgRenderObject* parent){
	return_if_fail(thiz && parent);
	thiz->has_fill = parent->has_fill;
	thiz->fill = parent->fill;
	thiz->fill_rule = parent->fill_rule;
	thiz->fill_opacity = parent->fill_opacity;
	thiz->stroke = parent->stroke;
	thiz->has_stroke = parent->has_stroke;
	thiz->stroke_width = parent->stroke_width;
	thiz->stroke_linecap = parent->stroke_linecap;
	thiz->stroke_linejoin = parent->stroke_linejoin;
	thiz->stroke_opacity = parent->stroke_opacity;
	COPY_NUM_ARRAY(thiz->stroke_dasharray, parent->stroke_dasharray);
	thiz->stroke_miterlimit = parent->stroke_miterlimit;
	thiz->stroke_dashoffset = parent->stroke_dashoffset;
	//memcpy(thiz->transform, parent->transform, sizeof(REAL) * 9);
	//原来的版本在继承的时候自动将transform置为父元素的transform，现在用cpp版本，transform可以不必继承。FIXME
	memcpy(thiz->transform, &unit_matrix, sizeof(NUMBER)*9); //单位矩阵
}

PRIVATE void parse_style_attribute(SvgRenderObject* thiz, LPCTSTR str){
	ATTR* attr = NULL;
	LPCTSTR style_name = NULL, style_value = NULL;
	return_if_fail(thiz && str);
	OMIT_WSP(str);
	while(*str){
		attr = (ATTR*)malloc(sizeof(ATTR));
		style_name = str;
		while(*str && !IS_WSP(*str) && *str != _T(':')) str++;
		//FIXME：这里不够完善，对于非正规的style会产生异常，留待完善。
		attr->name = get_substring(style_name, 0, str - style_name);
		OMIT_WSP(str);
		if(*str == _T(':')) str++;
		OMIT_WSP(str);
		style_value = str;
		while(*str && !IS_WSP(*str) && *str != _T(';')) str++;
		attr->value = get_substring(style_value, 0, str - style_value);
		get_standard_attribute(thiz, attr);
		if(*str == ';') str++;
		OMIT_WSP(str);
		attr_destroy(attr);
	}
}

PRIVATE ViewBox string_to_view_box(LPTSTR str){
	ViewBox view_box = {0};
	view_box.min_x = (NUMBER)string_to_number(str, &str);
	OMIT_COMMA_WSP(str);
	view_box.min_y = (NUMBER)string_to_number(str, &str);
	OMIT_COMMA_WSP(str);
	view_box.width = (NUMBER)string_to_number(str, &str);
	OMIT_COMMA_WSP(str);
	view_box.height = (NUMBER)string_to_number(str, &str);
	//FIXME:经过测试，viewBox只有输入了4个值的时候（多了少了都不行！），viewBox属性才有效。
	return view_box;
}

PRIVATE ARRAY string_to_dasharray(LPTSTR str){
	ARRAY dash_array = {0};
	LPTSTR start = NULL;
	NUMBER* cursor = NULL;
	BOOL repeat_twice = FALSE;
	OMIT_WSP(str);
	start = str;
	while(*str){
		OMIT_NUMBER(str);
		dash_array.length++;
		OMIT_COMMA_WSP(str);
	}

	if(dash_array.length % 2 != 0){
		repeat_twice = TRUE;
	}
	dash_array.data = malloc((repeat_twice? 2 : 1) * dash_array.length * sizeof(NUMBER));
	cursor = (NUMBER*)dash_array.data;
	str = start;
	while(*str){
		*cursor = string_to_number(str, &str);
		if(repeat_twice){
			*(cursor + dash_array.length) = *cursor;
		}
		cursor++;
		OMIT_COMMA_WSP(str);
	}
	if(repeat_twice) dash_array.length *= 2;
	return dash_array;
}

PRIVATE void string_to_preserve_aspect_ratio(LPTSTR str, PRESERVE_ASPECT_RATIO* par){
	LPTSTR start;
	int len, index;
	//PreserveAspectRatio is case-sensitive, and 'xMidYMid meet' is the default preserveAspectRatio.
	//<align>
	OMIT_WSP(str);
	start = str;
	OMIT_NONE_WSP(str);
	len = str - start;
	
	for (index = sizeof PreserveAspectRatioAlign / sizeof PreserveAspectRatioAlign[0] - 1; index >= 0; index--)
		if (!_tcsncmp(start, PreserveAspectRatioAlign[index].mnemonic, len))
			break;
	//if <align> is none, then the optional <meetOrSlice> value is ignored.
	//However, it should be noted that even if <align> is none, the <meetOrSlice> should be valid.
	if(index < 0) goto ERROR_PROCESSING;//invalid <align>, process error
	par->align = (PRESERVE_ASPECT_RATIO_ALIGN)PreserveAspectRatioAlign[index].token;
	//<meetOrSlice>
	OMIT_WSP(str);
	start = str;
	OMIT_NONE_WSP(str);
	len = str - start;

	if(!_tcsncmp(start, _T("meet"), len)){
		par->meet_or_slice = PRESERVE_ASPECT_RATIO_MORS_MEET;
	}else if(!_tcsncmp(start, _T("slice"), len)){
		par->meet_or_slice = PRESERVE_ASPECT_RATIO_MORS_SLICE;
	}else{
		goto ERROR_PROCESSING;//invalid <meetOrSlice>, process error
	}

	OMIT_WSP(str);
	if(*str) goto ERROR_PROCESSING;//redundant value, process error
	return; //return normally

ERROR_PROCESSING:
	//Invalid preserveAspectRatio appears. Set preserveAspectRatio as 'xMidYMid meet'
	par->align = PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMID;
	par->meet_or_slice = PRESERVE_ASPECT_RATIO_MORS_MEET;
}