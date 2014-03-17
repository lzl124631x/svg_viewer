#include "stdafx.h"
#include "SvgRenderer_GdiPlus.h"
#include <math.h>

#define OFFSET_POINT(pt, ref_pt) do{pt.X += ref_pt.X; pt.Y += ref_pt.Y;}while(0)
#define OFFSET_IF_RELATIVE(pt) if(relative_command) OFFSET_POINT(pt, current_point)
#define COLORREF_TO_ARGB(color, alpha) ((ARGB)(((BYTE)(0xFF * (alpha)) << 24) + (GetRValue(color) << 16)\
	+ (GetGValue(color) << 8)\
	+ GetBValue(color)))
/************************************************************************/
/*                            私有函数声明                              */
/************************************************************************/
PRIVATE void svg_renderer_render_object(SvgRenderObject* render_object, Graphics* graphics);
PRIVATE void set_clip_path(SvgClipPathRenderObject* clip_path_render_object, Graphics* graphics);
PRIVATE void add_path(SvgPathRenderObject* render_object, GraphicsPath* path);
PRIVATE void add_rect_path(SvgRectRenderObject* render_object, GraphicsPath* path);
PRIVATE void add_circle_path(SvgCircleRenderObject* render_object, GraphicsPath* path);
PRIVATE void add_ellipse_path(SvgEllipseRenderObject* render_object, GraphicsPath* path);
PRIVATE void add_line_path(SvgLineRenderObject* render_object, GraphicsPath* path);
PRIVATE void add_polyline_path(SvgPolylineRenderObject* render_object, GraphicsPath* path);
PRIVATE void add_polygon_path(SvgPolygonRenderObject* render_object, GraphicsPath* path);
PRIVATE void endpoint_to_center_parameterization(PointF pt1, PointF pt2, NUMBER fA, NUMBER fS, NUMBER rx, NUMBER ry, NUMBER phi,
	GraphicsPath* arc);
#define GET_VECTOR_ANGLE(x, y) (atan2((y), (x)) * DEGREE)
/************************************************************************/
/*                            外部函数实现                              */
/************************************************************************/
PUBLIC float svg_renderer_scale_ratio;
PUBLIC void svg_renderer_render_gdiplus(SvgRenderTree* render_tree, Graphics* graphics){
	return_if_fail(render_tree);
	svg_renderer_render_object(svg_render_tree_get_root(render_tree), graphics);
}

/************************************************************************/
/*                            私有函数实现                              */
/************************************************************************/

PRIVATE void svg_renderer_render_object(SvgRenderObject* render_object, Graphics* graphics){
	SvgRenderObject* child;
	BOOL is_container = FALSE;
	Pen pen(Color::Transparent, 0);
	SolidBrush brush(Color::Transparent);
	BOOL is_use_object = FALSE;
	GraphicsContainer container = graphics->BeginContainer();
	Matrix matrix;
	NUMBER* transform = NULL;
	GraphicsPath path(FillModeWinding);
	BOOL has_fill, has_stroke;
	SvgClipPathRenderObject* clip_path_render_object = NULL;
	return_if_fail(render_object);

	graphics->SetSmoothingMode(SmoothingModeHighQuality);

	//implement transform
	transform = svg_render_object_get_transform(render_object);
	matrix.SetElements(
		transform[0], transform[3], transform[1],
		transform[4], transform[2], transform[5]);
	graphics->MultiplyTransform(&matrix);

	//set pen for stroking
	has_stroke = svg_render_object_has_stroke((SvgRenderObject*)render_object);
	if(has_stroke){
		LineCap linecap = LineCapFlat;
		ARRAY dash_array = svg_render_object_get_stroke_dasharray(render_object);
		//stroke & stroke-opacity
		pen.SetColor(
			Color(
				COLORREF_TO_ARGB(
					svg_render_object_get_stroke_color(render_object), 
					svg_render_object_get_stroke_opacity(render_object))));
		//stroke-width
		pen.SetWidth(svg_render_object_get_stroke_width(render_object));
		//stroke-dasharray
		pen.SetDashStyle(DashStyleCustom);
		pen.SetDashPattern((float*)dash_array.data, dash_array.length);
		//stroke-dashoffset
		pen.SetDashOffset(svg_render_object_get_stroke_dashoffset(render_object));
		//stroke-linecap
		linecap = svg_render_object_get_stroke_linecap(render_object);
		pen.SetLineCap(linecap, linecap, DashCapFlat);
		//stroke-linejoin
		pen.SetLineJoin(svg_render_object_get_stroke_linejoin(render_object));
		//stroke-miterlimit
		pen.SetMiterLimit(svg_render_object_get_stroke_miterlimit(render_object));
	}
	//set brush for filling
	has_fill = svg_render_object_has_fill((SvgRenderObject*)render_object);
	if(has_fill){
		//fill & fill-opacity
		brush.SetColor(
			Color(
				COLORREF_TO_ARGB(
				svg_render_object_get_fill_color(render_object),
				svg_render_object_get_fill_opacity(render_object))));
		//fill-rule
		path.SetFillMode(svg_render_object_get_fill_rule((SvgRenderObject*) render_object));
	}

	//set clip path
	clip_path_render_object = svg_render_object_get_clip_path_element(render_object);
	if(clip_path_render_object){
		set_clip_path(clip_path_render_object, graphics);
	}

	//fetch information from render object
	switch(svg_render_object_get_type(render_object)){
	case SVG_TAG_TYPE_PATH: add_path((SvgPathRenderObject*)render_object, &path);break;
	case SVG_TAG_TYPE_RECT: add_rect_path((SvgRectRenderObject*)render_object, &path);break;
	case SVG_TAG_TYPE_CIRCLE: add_circle_path((SvgCircleRenderObject*)render_object, &path); break;
	case SVG_TAG_TYPE_ELLIPSE: add_ellipse_path((SvgEllipseRenderObject*)render_object, &path); break;
	case SVG_TAG_TYPE_LINE: add_line_path((SvgLineRenderObject*)render_object, &path);break;
	case SVG_TAG_TYPE_POLYLINE: add_polyline_path((SvgPolylineRenderObject*)render_object, &path);break;
	case SVG_TAG_TYPE_POLYGON: add_polygon_path((SvgPolygonRenderObject*)render_object, &path);break;
	case SVG_TAG_TYPE_SVG:{
		Pen red_pen(Color(128, 255, 0, 0));
		is_container = TRUE;
		LENGTH width = svg_svg_render_object_get_width((SvgSvgRenderObject*)render_object);
		LENGTH height = svg_svg_render_object_get_height((SvgSvgRenderObject*)render_object);
		if(width.unit_id == UNIT_ID_PERCENTAGE){
			width.number *= g_client_area.Width / svg_renderer_scale_ratio;// divide scale ratio to offset the scale transform
			width.unit_id = UNIT_ID_PX;
		}
		g_viewport_size.Width = width.number;//保存viewport宽度的计算值
		if(height.unit_id == UNIT_ID_PERCENTAGE){
			height.number *= g_client_area.Height / svg_renderer_scale_ratio;
			height.unit_id = UNIT_ID_PX;
		}
		g_viewport_size.Height = height.number;//保存viewport高度的计算值
		RectF rect(0, 0, width.number + 1, height.number + 1);//这里width和height各加一是为了正确clip
		graphics->DrawRectangle(&red_pen, rect);
		//set the initial clipping path
		graphics->SetClip(rect);

		if(svg_svg_render_object_has_view_box((SvgSvgRenderObject*)render_object)){
			ViewBox view_box = svg_svg_render_object_get_view_box((SvgSvgRenderObject*)render_object);
			PRESERVE_ASPECT_RATIO par = svg_svg_render_object_get_preserve_aspect_ratio((SvgSvgRenderObject*)render_object);
			NUMBER uniform_scale_ratio, x_scale_ratio, y_scale_ratio;
			x_scale_ratio = width.number / view_box.width;
			y_scale_ratio = height.number / view_box.height;
			if(par.meet_or_slice == PRESERVE_ASPECT_RATIO_MORS_MEET){
				uniform_scale_ratio = min(x_scale_ratio, y_scale_ratio);
			}else{
				uniform_scale_ratio = max(x_scale_ratio, y_scale_ratio);
			}
			if(par.align == PRESERVE_ASPECT_RATIO_ALIGN_NONE){
				//Stretch to fit non-uniformly.
				graphics->ScaleTransform(x_scale_ratio, y_scale_ratio);
			}else{
				//Force uniform scaling.
				graphics->ScaleTransform(uniform_scale_ratio, uniform_scale_ratio);
				switch(par.align){
				case PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMIN:
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMIN:
					graphics->TranslateTransform((width.number / uniform_scale_ratio - view_box.width) / 2, 0);
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMIN:
					graphics->TranslateTransform(width.number / uniform_scale_ratio - view_box.width, 0);
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMID:
					graphics->TranslateTransform(0, (height.number / uniform_scale_ratio - view_box.height) / 2);
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMID:
					graphics->TranslateTransform(
						(width.number / uniform_scale_ratio - view_box.width) / 2, 
						(height.number / uniform_scale_ratio - view_box.height) / 2);
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMID:
					graphics->TranslateTransform(
						width.number / uniform_scale_ratio - view_box.width, 
						(height.number / uniform_scale_ratio - view_box.height) / 2);
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMIN_YMAX:
					graphics->TranslateTransform(0, height.number / uniform_scale_ratio - view_box.height);
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMID_YMAX:
					graphics->TranslateTransform(
						(width.number / uniform_scale_ratio - view_box.width) / 2, 
						height.number / uniform_scale_ratio - view_box.height);
					break;
				case PRESERVE_ASPECT_RATIO_ALIGN_XMAX_YMAX:;
					graphics->TranslateTransform(
						width.number / uniform_scale_ratio - view_box.width, 
						height.number / uniform_scale_ratio - view_box.height);
					break;
				}//switch
			}//else
			graphics->TranslateTransform(-view_box.min_x, -view_box.min_y);
		}//if
		break;
						  }
	case SVG_TAG_TYPE_G: is_container = TRUE; break;
	case SVG_TAG_TYPE_USE:
		is_container = TRUE;
		graphics->TranslateTransform(
			(float)svg_use_render_object_get_x((SvgUseRenderObject*)render_object), 
			(float)svg_use_render_object_get_y((SvgUseRenderObject*)render_object));
		 break;
	}

	//rendering
	if(!is_container){//render myself
		if(has_fill) graphics->FillPath(&brush, &path);
		if(has_stroke) graphics->DrawPath(&pen, &path);
	}else{//render child
		child = svg_render_object_get_first_child(render_object);
		while(is_container && child){
			svg_renderer_render_object(child, graphics);
			child = svg_render_object_get_next_sibling(child);
		}
	}
	graphics->EndContainer(container);
}

PRIVATE void set_clip_path(SvgClipPathRenderObject* clip_path_render_object, Graphics* graphics){
	SvgRenderObject* child = NULL;
	GraphicsPath clip_path;
	return_if_fail(clip_path_render_object && graphics);

	child = svg_render_object_get_first_child((SvgRenderObject*)clip_path_render_object);
	while(child){
		clip_path.Reset();
		switch(svg_render_object_get_type(child)){
		case SVG_TAG_TYPE_PATH: add_path((SvgPathRenderObject*)child, &clip_path); graphics->IntersectClip(&Region(&clip_path)); break;
		case SVG_TAG_TYPE_RECT: add_rect_path((SvgRectRenderObject*)child, &clip_path); graphics->IntersectClip(&Region(&clip_path)); break;
		case SVG_TAG_TYPE_CIRCLE: add_circle_path((SvgCircleRenderObject*)child, &clip_path); graphics->IntersectClip(&Region(&clip_path)); break;
		case SVG_TAG_TYPE_ELLIPSE: add_ellipse_path((SvgEllipseRenderObject*)child, &clip_path); graphics->IntersectClip(&Region(&clip_path)); break;
		case SVG_TAG_TYPE_LINE: add_line_path((SvgLineRenderObject*)child, &clip_path); graphics->IntersectClip(&Region(&clip_path)); break;
		case SVG_TAG_TYPE_POLYLINE: add_polyline_path((SvgPolylineRenderObject*)child, &clip_path); graphics->IntersectClip(&Region(&clip_path)); break;
		case SVG_TAG_TYPE_POLYGON: add_polygon_path((SvgPolygonRenderObject*)child, &clip_path); graphics->IntersectClip(&Region(&clip_path));break;
		}
		child = svg_render_object_get_next_sibling(child);
	}
}

PRIVATE void add_path(SvgPathRenderObject* render_object, GraphicsPath* path){
	ARRAY path_data = {0};
	PathCommand path_cmd = {0};
	int i, j, data_num;
	NUMBER* data_array = NULL;
	TCHAR last_command_type = 0;//用于暂存上一条路径命令类型

	PointF last_control_point;//用于存储上一个控制点，方便S、T
	PointF current_point;//用于存当前点的坐标值，辅助画线
	PointF new_point;//存储新产生的点，用于传递给current point
	PointF point_group[3];//用于辅助DrawBezierTo
	BOOL relative_command;

	return_if_fail(render_object && path);

	path_data = svg_path_render_object_get_path_data(render_object);

	//开始绘制
	for(i = 0; i < path_data.length; i++){
		path_cmd = ((PathCommand*)path_data.data)[i];
		data_num = path_cmd.data_array.length;
		data_array = (NUMBER*)path_cmd.data_array.data;
		j = 0;
		relative_command = FALSE;
		switch(path_cmd.cmd_type){
		case 'm': relative_command = TRUE;
		case 'M':{//移动当前点
			BOOL is_first_point = TRUE;
			//这里应该检测data个数为偶数
			while(j < data_num){
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				if(is_first_point){
					path->StartFigure();
					is_first_point = FALSE;
				}else{
					path->AddLine(current_point, new_point);
				}
				current_point = new_point;
			}
			break;
				 }//case
		case 'z':
		case 'Z':{//闭合曲线，z和Z等效。
			path->CloseFigure();
			break;
				 }
		case 'l': relative_command = TRUE;
		case 'L':{
			//这里应该检测data个数为偶数
			while(j < data_num){
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				path->AddLine(current_point, new_point);
				current_point = new_point;
			}
			break;
				 }//case
		case 'h': relative_command = TRUE;
		case 'H':{
			//这里应该检测data个数为奇数
			while(j < data_num){
				new_point.X = data_array[j++];
				new_point.Y = current_point.Y;
				if(relative_command){
					new_point.X += current_point.X;
				}
				path->AddLine(current_point, new_point);
				current_point = new_point;
			}
			break;
				 }//case
		case 'v': relative_command = TRUE;
		case 'V':{
			//这里应该检测data个数为奇数
			while(j < data_num){		
				new_point.X = current_point.X;
				new_point.Y = data_array[j++];
				if(relative_command){
					new_point.Y += current_point.Y;
				}
				path->AddLine(current_point, new_point);
				current_point = new_point;
			}
			break;
				 }//case
		case 'c': relative_command = TRUE;
		case 'C':{//Cubic Bezier
			//这里应该检测data个数为偶数
			while(j < data_num){
				//第一个控制点
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				point_group[0] = new_point;

				//第二个控制点
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				last_control_point = new_point;
				point_group[1] = new_point;

				//结束点
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				point_group[2] = new_point;

				path->AddBezier(current_point, point_group[0], point_group[1], point_group[2]);
				current_point = new_point;
			}
			break;
				 }//case
		case 's': relative_command = TRUE;
		case 'S':{
			while(j < data_num){	
				//第一个控制点
				if(last_command_type == 'c'
					|| last_command_type == 'C' 
					|| last_command_type == 's' 
					|| last_command_type == 'S'){
						//第一个控制点被设为上一个命令的第二个控制点关于current point的对称点。
						new_point.X = 2 * current_point.X - last_control_point.X;
						new_point.Y = 2 * current_point.Y - last_control_point.Y;
				}else{
					//如果没有上一个命令或者上一个命令不是C，c，S或s，那么第一个控制点被设为current point。
					new_point.X = current_point.X;
					new_point.Y = current_point.Y;
				}
				//FIXME:若上一个命令不是CcSs，且当前命令是小写的要如何处理？
				point_group[0] = new_point;

				//第二个控制点
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				last_control_point = new_point;
				point_group[1] = new_point;

				//结束点
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				point_group[2] = new_point;

				path->AddBezier(current_point, point_group[0], point_group[1], point_group[2]);
				current_point = new_point;
			}
			break;
				 }//case
		case 'q': relative_command = TRUE;
		case 'Q':{//Quadratic Bezier
			PointF q0, q1, q2;
			while(j < data_num){
				//二次贝塞尔的起始点q0
				q0.X = current_point.X;
				q0.Y = current_point.Y;

				//二次贝塞尔控制点q1
				q1.X = data_array[j++];
				q1.Y = data_array[j++];
				OFFSET_IF_RELATIVE(q1);
				last_control_point = q1;

				//三次贝塞尔的控制点c1 = (q0 + 2 * q1)/3 = q0 + 2 * (q1 - q0)/3
				new_point.X = (q0.X + 2 * q1.X)/3.0f;
				new_point.Y = (q0.Y + 2 * q1.Y)/3.0f;
				point_group[0] = new_point;

				//二次贝塞尔结束点q2
				q2.X = data_array[j++];
				q2.Y = data_array[j++];

				OFFSET_IF_RELATIVE(q2);

				//三次贝塞尔的控制点c2 = (2 * q1 + q2)/3 = c1 + (q2 - q0)/3
				new_point.X = (2 * q1.X + q2.X)/3.0f;
				new_point.Y = (2 * q1.Y + q2.Y)/3.0f;
				point_group[1] = new_point;

				//三次贝塞尔的结束点c3 = q2
				point_group[2] = q2;

				path->AddBezier(current_point, point_group[0], point_group[1], point_group[2]);
				current_point = point_group[2];
			}
			break;
				 }//case
		case 't': relative_command = TRUE;
		case 'T':{
			PointF q0, q1, q2;
			while(j < data_num){
				//二次贝塞尔的起始点q0
				q0.X = current_point.X;
				q0.Y = current_point.Y;

				//二次贝塞尔的控制点q1
				if(last_command_type == 'q'
					||last_command_type == 'Q'
					||last_command_type == 't'
					||last_command_type == 'T'){
						//控制点被设为上一个命令的最后一个控制点关于current point的对称点。
						q1.X = 2 * current_point.X - last_control_point.X;
						q1.Y = 2 * current_point.Y - last_control_point.Y;
				}else{
					//如果没有上一个命令或者上一个命令不是C，c，S或s，那么控制点被设为current point。
					q1.X = current_point.X;
					q1.Y = current_point.Y;
				}
				last_control_point = q1;

				//三次贝塞尔的控制点c1 = (q0 + 2 * q1)/3 = q0 + 2 * (q1 - q0)/3
				new_point.X = (q0.X + 2 * q1.X) / 3.0f;
				new_point.Y = (q0.Y + 2 * q1.Y) / 3.0f;
				point_group[0] = new_point;

				//二次贝塞尔的结束点q2
				q2.X = data_array[j++];
				q2.Y = data_array[j++];
				OFFSET_IF_RELATIVE(q2);

				//三次贝塞尔的控制点c2 = (2 * q1 + q2)/3 = c1 + (q2 - q0)/3
				new_point.X = (2 * q1.X + q2.X) / 3.0f;
				new_point.Y = (2 * q1.Y + q2.Y) / 3.0f;
				point_group[1] = new_point;

				//三次贝赛尔的结束点c3 = q2
				point_group[2] = q2;

				path->AddBezier(current_point, point_group[0], point_group[1], point_group[2]);
				current_point = point_group[2];
			}
			break;
				 }//case
		case 'a': relative_command = TRUE;
		case 'A':{
			NUMBER rx, ry, x_axis_rotation, large_arc_flag, sweep_flag;
			GraphicsPath arc;
			while(j < data_num){
				// (rx, ry)
				rx = data_array[j++];
				ry = data_array[j++];
				// x-axis-rotation
				x_axis_rotation = data_array[j++];
				// large-arc-flag
				large_arc_flag = data_array[j++];
				// sweep-flag
				sweep_flag = data_array[j++];
				// (x, y)
				new_point.X = data_array[j++];
				new_point.Y = data_array[j++];
				OFFSET_IF_RELATIVE(new_point);
				if(rx == 0 || ry == 0){// If rx = 0 or ry = 0, then treat this as a straight line from (x1, y1) to (x2, y2) and stop.
					path->AddLine(current_point, new_point);
				}else{
					//Ensure radii are positive
					if(rx < 0) rx = -rx;
					if(ry < 0) ry = -ry;
					endpoint_to_center_parameterization(current_point, new_point, large_arc_flag, sweep_flag, rx, ry, x_axis_rotation,
						&arc);
					path->AddPath(&arc, TRUE);
				}
				current_point = new_point;
			}
			break;
				 }//case
		}
		last_command_type = path_cmd.cmd_type;
	}
}

#define ADJUST_X(x) if(x.unit_id == UNIT_ID_PERCENTAGE){x.number *= g_viewport_size.Width;}
#define ADJUST_Y(y) if(x.unit_id == UNIT_ID_PERCENTAGE){y.number *= g_viewport_size.Height;}
PRIVATE void add_rect_path(SvgRectRenderObject* render_object, GraphicsPath* path){
	COORDINATE x, y;
	LENGTH width, height, rx, ry;
	return_if_fail(render_object);
	width = svg_rect_render_object_get_width(render_object);
	height = svg_rect_render_object_get_height(render_object);
	x = svg_rect_render_object_get_x(render_object);
	y = svg_rect_render_object_get_y(render_object);
	rx = svg_rect_render_object_get_rx(render_object);
	ry = svg_rect_render_object_get_ry(render_object);
	ADJUST_X(width);
	ADJUST_Y(height);
	ADJUST_X(x);
	ADJUST_Y(y);
	ADJUST_X(rx);
	ADJUST_Y(ry);
	if(rx.number == 0 || ry.number == 0){
		rx.number = ry.number = 0;
	}else{
		if(rx.number > width.number / 2){
			rx.number = width.number / 2;
		}
		if(ry.number > height.number / 2){
			ry.number = height.number / 2;
		}
	}
	path->StartFigure();
	path->AddLine(x.number + rx.number, y.number, x.number + width.number - rx.number, y.number);//top border
	path->AddArc(x.number + width.number - 2 * rx.number, y.number, 2 * rx.number, 2 * ry.number, -90, 90);//top right corner
	path->AddLine(x.number + width.number, y.number + ry.number, x.number + width.number, y.number + height.number - ry.number);//right border
	path->AddArc(x.number + width.number - 2 * rx.number, y.number + height.number - 2 * ry.number, 2 * rx.number, 2 * ry.number, 0, 90);//bottom right corner
	path->AddLine(x.number + width.number - rx.number, y.number + height.number, x.number + rx.number, y.number + height.number);//bottom border
	path->AddArc(x.number, y.number + height.number - 2 * ry.number, 2 * rx.number, 2 * ry.number, 90, 90);//bottom left corner
	path->AddLine(x.number, y.number + height.number - ry.number, x.number, y.number + ry.number);//left border
	path->AddArc(x.number, y.number, 2 * rx.number, 2 * ry.number, -180, 90);//top left corner
	path->CloseFigure();
}

PRIVATE void add_circle_path(SvgCircleRenderObject* render_object, GraphicsPath* path){
	NUMBER cx, cy, radius;
	return_if_fail(render_object && path);
	cx = svg_circle_render_object_get_center_x(render_object);
	cy = svg_circle_render_object_get_center_y(render_object);
	radius = svg_circle_render_object_get_radius(render_object);
	path->AddEllipse(cx - radius, cy - radius, radius * 2, radius * 2);
}

PRIVATE void add_ellipse_path(SvgEllipseRenderObject* render_object, GraphicsPath* path){
	NUMBER cx, cy, rx, ry;
	return_if_fail(render_object && path);
	cx = svg_ellipse_render_object_get_center_x(render_object);
	cy = svg_ellipse_render_object_get_center_y(render_object);
	rx = svg_ellipse_render_object_get_radius_x(render_object);
	ry = svg_ellipse_render_object_get_radius_y(render_object);
	path->AddEllipse(cx - rx, cy - ry, rx * 2, ry * 2);
}

PRIVATE void add_line_path(SvgLineRenderObject* render_object, GraphicsPath* path){
	NUMBER x1, y1, x2, y2;
	return_if_fail(render_object && path);
	x1 = svg_line_render_object_get_x1(render_object);
	y1 = svg_line_render_object_get_y1(render_object);
	x2 = svg_line_render_object_get_x2(render_object);
	y2 = svg_line_render_object_get_y2(render_object);
	path->AddLine(x1, y1, x2, y2);
}

PRIVATE void add_polyline_path(SvgPolylineRenderObject* render_object, GraphicsPath* path){
	ARRAY point_array = {0};
	return_if_fail(render_object && path);

	point_array = svg_polyline_render_object_get_point_array(render_object);
	path->AddLines((PointF*)point_array.data, point_array.length);
}

PRIVATE void add_polygon_path(SvgPolygonRenderObject* render_object, GraphicsPath* path){
	ARRAY point_array = {0};
	return_if_fail(render_object && path);

	point_array = svg_polygon_render_object_get_point_array(render_object);
	path->AddPolygon((PointF*)point_array.data, point_array.length);
}

#undef OFFSET_POINT
#undef OFFSET_IF_RELATIVE
#undef COLORREF_TO_ARGB

PRIVATE void endpoint_to_center_parameterization(PointF pt1, PointF pt2, NUMBER fA, NUMBER fS, NUMBER rx, NUMBER ry, NUMBER phi,
	GraphicsPath* arc){
	NUMBER psi1, delta_psi;
	Matrix arc_rot;
	NUMBER x1_, y1_, cos_phi, sin_phi, cx_, cy_, cx, cy;
	NUMBER lambda; 
	cos_phi = cos(phi / DEGREE);
	sin_phi = sin(phi / DEGREE);
	// Step 1: Compute (x1', y1')
	x1_ = cos_phi * (pt1.X - pt2.X) / 2 + sin_phi * (pt1.Y - pt2.Y) / 2;
	y1_ = -sin_phi * (pt1.X - pt2.X) / 2 + cos_phi * (pt1.Y - pt2.Y) / 2;
	// Correction of out-of-range radii
	lambda = x1_*x1_ / (rx*rx) + y1_*y1_ / (ry*ry);
	if(lambda > 1){
		// Ensure radii are large enough
		rx *= sqrt(lambda);
		ry *= sqrt(lambda);
		cx = (pt1.X + pt2.X) / 2;
		cy = (pt1.Y + pt2.Y) / 2;
		psi1 = GET_VECTOR_ANGLE(pt1.X - pt2.X, pt1.Y - pt2.Y) - phi;
		delta_psi = 180;
	}else{
		// Step 2: Compute (cx', cy')
		lambda = sqrt(1 / lambda - 1);
		cx_ = lambda * rx * y1_ / ry;
		cy_ = -lambda * ry * x1_ / rx;
		if(fA == fS){
			cx_ = -cx_;
			cy_ = -cy_;
		}
		// Step 3: Compute (cx, cy)
		cx = cos_phi * cx_ - sin_phi * cy_ + (pt1.X + pt2.X) / 2;
		cy = sin_phi * cx_ + cos_phi * cy_ + (pt1.Y + pt2.Y) / 2;
		// Step 4: Compute \theta_1 and \Delta\theta
		psi1 = GET_VECTOR_ANGLE(x1_ - cx_, y1_ - cy_);
		delta_psi = GET_VECTOR_ANGLE(-x1_ - cx_, -y1_ - cy_) - psi1;
	}
	if(fS == 0 && delta_psi > 0){
		delta_psi -= 360;
	}else if(fS == 1 && delta_psi < 0){
		delta_psi += 360;
	}
	arc->AddArc(cx - rx, cy - ry, 2 * rx, 2 * ry, psi1, delta_psi);
	arc_rot.RotateAt(phi, PointF(cx, cy));
	arc->Transform(&arc_rot);
}