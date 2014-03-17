// SVGViewer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SvgViewer.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
/************************************************************************/
/*                          全局变量                                    */
/************************************************************************/
LPCTSTR g_root_dir = _T("D:/ALL SVG SAMPLE");//保存尝试打开SVG文件时的默认根目录
TCHAR g_svg_file_name[MAX_PATH] = {0};//保存打开的SVG文件的路径

const float g_scale_ratios[16] = {
	0.25f, 0.33f, 0.50f, 0.67f, 0.75f, 0.90f, 1.00f,
	1.10f, 1.25f, 1.50f, 1.75f, 2.00f, 2.50f, 3.00f, 4.00f, 5.00f
};
#define MIN_SCALE_INDEX 0
#define MAX_SCALE_INDEX 15
#define DEFAULT_SCALE_INDEX 6
int g_scale_ratio_index = DEFAULT_SCALE_INDEX;

clock_t g_start, g_after_parsing, g_after_render_tree, g_before_rendering, g_after_rendering;
float g_parsing, g_building_render_tree, g_rendering;

BOOL g_control_is_down = FALSE;

TCHAR g_status[100] = {0};

SCROLLINFO vert_si = {sizeof(SCROLLINFO), SIF_ALL};
SCROLLINFO horz_si = {sizeof(SCROLLINFO), SIF_ALL};

#define REFRESH_WINDOW do{ InvalidateRect(hWnd, 0, 1); UpdateWindow(hWnd); }while(0)

#ifdef GDI_PLUS
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_pGdiToken;
#endif
/************************************************************************/
/*                         私有函数声明                                 */
/************************************************************************/
PRIVATE void reparse_svg_and_refresh_window(HWND hWnd);
PRIVATE void print_status(HDC hdc);
PRIVATE void resize_scroll_bar(HWND hWnd);
PRIVATE void relocate_scroll_bar(HWND hWnd);
#define RESET_SCROLL_BAR(hWnd) do{ resize_scroll_bar(hWnd); relocate_scroll_bar(hWnd);}while(0)
/************************************************************************/
/*                         窗体函数主体                                 */
/************************************************************************/
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SVGVIEWER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SVGVIEWER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SVGVIEWER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SVGVIEWER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SVGVIEWER));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   hInst = hInstance; // Store instance handle in our global variable

#ifdef GDI_PLUS
   //启动GDI+
   GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL);
#endif

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //隐藏滚动条
   SetScrollInfo(hWnd, SB_VERT, &vert_si, TRUE);
   SetScrollInfo(hWnd, SB_HORZ, &horz_si, TRUE);

   DragAcceptFiles(hWnd,true);//这里告诉Windows窗口hWnd可以接受从shell拖放过来的文件。
   //获取当前DC的DPI
   HDC hdc = GetDC(hWnd);
   dpi_x = GetDeviceCaps (hdc, LOGPIXELSX);
   dpi_y = GetDeviceCaps (hdc, LOGPIXELSY);
   ReleaseDC (hWnd, hdc); 

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static int last_x = 0, last_y = 0;
	static int x_pos = 0, y_pos = 0;

	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(OPENFILENAME);//指定这个结构的大小，以字节为单位。
	ofn.lpstrFilter = _T("SVG文件(*.svg)\0*.svg\0");//指向一对以空字符结束的过滤字符串的一个缓冲。缓冲中的最后一个字符串必须以两个NULL字符结束。
	ofn.lpstrFile = g_svg_file_name;//指向包含初始化文件名编辑控件使用的文件名的缓冲。
	ofn.nMaxFile = MAX_PATH;//指定lpstrFile缓冲的大小，以字节为单位。
	ofn.Flags = OFN_FILEMUSTEXIST;//位标记的设置。OFN_FILEMUSTEXIST：指定用户仅可以在文件名登录字段中输入已存在的文件的名字。
	ofn.hwndOwner = hWnd;//指向所有者对话框窗口的句柄。
	ofn.lpstrInitialDir = g_root_dir;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		/************************************************************************/
		/*                         FILE OPEN EVENT                              */
		/************************************************************************/
		case IDM_FILE_OPEN:{
			if(GetOpenFileName(&ofn)){//strFile得到用户所选择文件的路径和文件名
				reparse_svg_and_refresh_window(hWnd);
			}
			break;
						   }
		/************************************************************************/
		/*                        FILE CLOSE EVENT                              */
		/************************************************************************/
		case IDM_FILE_CLOSE:{
			svg_dom_destroy(g_svg_dom);
			g_svg_dom = NULL;
			svg_render_tree_destroy(g_render_tree);
			g_render_tree = NULL;
			g_svg_file_name[0] = 0;
			REFRESH_WINDOW;
			break;
							}
		/************************************************************************/
		/*                          REFRESH EVENT                               */
		/************************************************************************/
		case IDM_FILE_REFRESH:{
			reparse_svg_and_refresh_window(hWnd);
			break;
							  }
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		/************************************************************************/
		/*                          PAINT EVENT                                 */
		/************************************************************************/
	case WM_PAINT:{
#ifdef DOUBLE_BUFFERING
		HDC buf_hdc;
		HBITMAP bufBmp, oldBmp;
		g_before_rendering = clock();

		//得到当前HDC
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		//创建内存HDC
		buf_hdc = CreateCompatibleDC(hdc);//创建兼容HDC
		//FIXME:创建位图，这里有个问题，能否创建一个初始为白色的bitmap
		bufBmp = CreateCompatibleBitmap(hdc, g_client_area.Width, g_client_area.Height);//创建兼容BITMAP
		oldBmp = (HBITMAP)SelectObject(buf_hdc, bufBmp);

		//绘图
#ifndef GDI_PLUS
		FillRect(buf_hdc, &client_rect, WHITE_BRUSH);//清空背景色
		svg_renderer_render(g_render_tree, buf_hdc);
#else
		Graphics graphics(buf_hdc);
		graphics.SetSmoothingMode(SmoothingModeHighQuality);
		graphics.Clear(Color::White);//清空背景色

		svg_renderer_scale_ratio = g_scale_ratios[g_scale_ratio_index];
		graphics.ScaleTransform(svg_renderer_scale_ratio, svg_renderer_scale_ratio);
		graphics.TranslateTransform((float)-horz_si.nPos, (float)-vert_si.nPos, MatrixOrderAppend);

		svg_renderer_render_gdiplus(g_render_tree, &graphics);

		print_status(buf_hdc);
#endif
		//拷贝内存HDC内容到实际HDC`
		BitBlt(hdc, 0, 0, g_client_area.Width, g_client_area.Height, buf_hdc, 0, 0, SRCCOPY);
		//内存回收
		SelectObject(buf_hdc, oldBmp);
		DeleteObject(bufBmp);
		DeleteDC(buf_hdc);
		EndPaint(hWnd, &ps);
		g_after_rendering = clock();
		g_rendering = (float)(g_after_rendering - g_before_rendering) / CLOCKS_PER_SEC;
		break;
#else
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		svg_renderer_render(g_render_tree, hdc);
		EndPaint(hWnd, &ps);
		break;
#endif
				  }
#ifdef DOUBLE_BUFFERING
	case WM_ERASEBKGND:
		break;
#endif
		/************************************************************************/
		/*                        DROP FILE EVENT                               */
		/************************************************************************/
	case WM_DROPFILES:{
		DragQueryFile((HDROP)wParam, 0, g_svg_file_name, MAX_PATH);//第二个参数为0表示同时拖拽多个文件时只接受第一个文件。
		//FIXME:这里需要判断一下文件的后缀名。而且未来要完善解析部分，确保不产生异常。
		reparse_svg_and_refresh_window(hWnd);
		DragFinish((HDROP)wParam);
		break;
					  }
		/************************************************************************/
		/*                      MOUSE WHEEL EVENT                               */
		/************************************************************************/
	case WM_MOUSEWHEEL:{
		WORD fw_keys = LOWORD(wParam);//key flags
		short z_delta = (short)HIWORD(wParam);//wheel rotation   
		if(fw_keys == MK_CONTROL){//scale
			if((z_delta < 0 && g_scale_ratio_index == MIN_SCALE_INDEX)
				|| z_delta > 0 && g_scale_ratio_index == MAX_SCALE_INDEX){
					return 0; // cannot scale anymore, return
			}
			g_scale_ratio_index += z_delta / 120;
			if(g_scale_ratio_index < MIN_SCALE_INDEX){
				g_scale_ratio_index = MIN_SCALE_INDEX;
			}else if(g_scale_ratio_index > MAX_SCALE_INDEX){
				g_scale_ratio_index = MAX_SCALE_INDEX;
			}
			resize_scroll_bar(hWnd);
			REFRESH_WINDOW;

		}else{//vertical scroll
			if(!(z_delta < 0 && vert_si.nPos + vert_si.nPage == vert_si.nMax)
				&& !(z_delta > 0 && vert_si.nPos == vert_si.nMin)){
					vert_si.nPos -= z_delta / 3;
					if(vert_si.nPos < vert_si.nMin){
						vert_si.nPos = vert_si.nMin;
					}else if(vert_si.nPos + (int)vert_si.nPage > vert_si.nMax){
						vert_si.nPos = vert_si.nMax - vert_si.nPage;
					}
					vert_si.fMask = SIF_POS;
					SetScrollInfo(hWnd, SB_VERT, &vert_si, TRUE);
					REFRESH_WINDOW;
			}
		}
		break;
					   }
	//	/************************************************************************/
	//	/*                    LEFT BUTTON DOWN EVENT                            */
	//	/************************************************************************/
	//case WM_LBUTTONDOWN:{
	//	if(wParam == (MK_CONTROL | MK_LBUTTON)){
	//		last_x = LOWORD(lParam);
	//		last_y = HIWORD(lParam);
	//	}
	//	break;
	//					}
	//	/************************************************************************/
	//	/*                       MOUSE MOVE EVENT                               */
	//	/************************************************************************/
	//case WM_MOUSEMOVE:{
	//	x_pos = LOWORD(lParam);
	//	y_pos = HIWORD(lParam);
	//	if(wParam == (MK_CONTROL | MK_LBUTTON)){
	//		horz_si.nPos -= x_pos - last_x;
	//		vert_si.nPos -= y_pos - last_y;
	//		REFRESH_WINDOW;
	//	}
	//	last_x = x_pos;
	//	last_y = y_pos;
	//	break;
	//				  }
	//	/************************************************************************/
	//	/*                    LEFT BUTTON UP EVENT                              */
	//	/************************************************************************/
	//case WM_LBUTTONUP:{
	//	if(wParam == MK_CONTROL){
	//		g_x_trans += x_pos - last_x;
	//		g_y_trans += y_pos - last_y;
	//		last_x = last_y = x_pos = y_pos = 0;
	//	}
	//	break;
	//				  }
		/************************************************************************/
		/*                      KEY DOWN EVENT                                  */
		/************************************************************************/
	case WM_KEYDOWN:{
		switch(wParam){
		case VK_CONTROL: 
			g_control_is_down = TRUE;
			break;
		case VK_OEM_PLUS: 
			if(g_control_is_down){
				g_scale_ratio_index += 1;
				if(g_scale_ratio_index > MAX_SCALE_INDEX){
					g_scale_ratio_index = MAX_SCALE_INDEX;
				}else{
					REFRESH_WINDOW;
				}
			}
			break;
		case VK_OEM_MINUS:
			if(g_control_is_down){
				g_scale_ratio_index -= 1;
				if(g_scale_ratio_index < MIN_SCALE_INDEX){
					g_scale_ratio_index = MIN_SCALE_INDEX;
				}else{
					REFRESH_WINDOW;
				}
			}
			break;
		case '0':
			if(g_control_is_down){
				g_scale_ratio_index = DEFAULT_SCALE_INDEX;
				horz_si.nPos = vert_si.nPos = 0;
				REFRESH_WINDOW;
			}
			break;
		case VK_F5:
			reparse_svg_and_refresh_window(hWnd);
			break;
		case VK_UP:
			vert_si.nPos -= vert_si.nPage / 10;
			REFRESH_WINDOW;
			break;
		case VK_DOWN:
			vert_si.nPos += vert_si.nPage / 10;
			REFRESH_WINDOW;
			break;
		case VK_LEFT:
			horz_si.nPos -= horz_si.nPos / 10;
			REFRESH_WINDOW;
			break;
		case VK_RIGHT:
			horz_si.nPos += horz_si.nPos / 10;
			REFRESH_WINDOW;
			break;
		}
		break;
					}
	case WM_KEYUP:{
		if(wParam == VK_CONTROL){
			g_control_is_down = FALSE;
		}
		break;
				  }
		/************************************************************************/
		/*                      VERTICAL SCROLL EVENT                           */
		/************************************************************************/
	case WM_VSCROLL:{
		switch(LOWORD(wParam)){
		case SB_LINEDOWN:
			if(vert_si.nPos == vert_si.nMax - (int)vert_si.nPage) return 0; //reach bottom already, return
			vert_si.nPos += (int)min(vert_si.nPage / 10, vert_si.nMax - vert_si.nPos - vert_si.nPage);
			break;
		case SB_LINEUP:
			if(vert_si.nPos == vert_si.nMin) return 0; //reach top already, return
			vert_si.nPos -= min((int)vert_si.nPage / 10, vert_si.nPos);
			break;
		case SB_PAGEDOWN:
			if(vert_si.nPos == vert_si.nMax - (int)vert_si.nPage) return 0; //reach bottom already, return
			vert_si.nPos += (int)min(vert_si.nPage, vert_si.nMax - vert_si.nPos - vert_si.nPage);
			break;
		case SB_PAGEUP:
			if(vert_si.nPos == vert_si.nMin) return 0; //reach top already, return
			vert_si.nPos -= min((int)vert_si.nPage, vert_si.nPos);
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			vert_si.fMask = SIF_TRACKPOS;
			GetScrollInfo(hWnd, SB_VERT, &vert_si);
			vert_si.nPos = vert_si.nTrackPos;
			break;
		}
		vert_si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &vert_si, TRUE);
		REFRESH_WINDOW;

		break;
					 }
		/************************************************************************/
		/*                    HORIZONTAL SCROLL EVENT                           */
		/************************************************************************/
	case WM_HSCROLL:{
		switch(LOWORD(wParam)){
		case SB_LINERIGHT:
			if(horz_si.nPos == horz_si.nMax - (int)horz_si.nPage) return 0; //reach right border already, return
			horz_si.nPos += (int)min(horz_si.nPage / 10, horz_si.nMax - horz_si.nPos - horz_si.nPage);
			break;
		case SB_LINELEFT:
			if(horz_si.nPos == horz_si.nMin) return 0; //reach left border already, return
			horz_si.nPos -= min((int)horz_si.nPage / 10, horz_si.nPos);
			break;
		case SB_PAGERIGHT:
			if(horz_si.nPos == horz_si.nMax - (int)horz_si.nPage) return 0; //reach right border already, return
			horz_si.nPos += (int)min(horz_si.nPage, horz_si.nMax - horz_si.nPos - horz_si.nPage);
			break;
		case SB_PAGELEFT:
			if(horz_si.nPos == horz_si.nMin) return 0; //reach left border already, return
			horz_si.nPos -= min((int)horz_si.nPage, horz_si.nPos);
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			horz_si.fMask = SIF_TRACKPOS;
			GetScrollInfo(hWnd, SB_HORZ, &horz_si);
			horz_si.nPos = horz_si.nTrackPos;
			break;
		}
		horz_si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_HORZ, &horz_si, TRUE);
		REFRESH_WINDOW;
		break;
					}
		/************************************************************************/
		/*                      WINDOW RESIZED EVENT                            */
		/************************************************************************/
	case WM_SIZE:{
		g_client_area.Width = (int)LOWORD(lParam);
		g_client_area.Height = (int)HIWORD(lParam);

		resize_scroll_bar(hWnd);
		break;
				 }
		/************************************************************************/
		/*                       WINDOW MOVED EVENT                             */
		/************************************************************************/
	case WM_MOVE:{
		g_client_area.X = (int)LOWORD(lParam);
		g_client_area.Y = (int)HIWORD(lParam);
		break;
				 }
		/************************************************************************/
		/*                      DESTROY EVENT                                   */
		/************************************************************************/
	case WM_DESTROY:
#ifdef GDI_PLUS
		GdiplusShutdown(m_pGdiToken);
#endif
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

/************************************************************************/
/*                         私有函数实现                               */
/************************************************************************/
PRIVATE void reparse_svg_and_refresh_window(HWND hWnd){
	//此函数的任务是解析svg文件并触发窗体的刷新操作。由于绘制操作不在这里，因此必须将解析结果保存到全局变量中。
	SvgRenderTreeBuilder* render_tree_builder = NULL;
	//析构上一次的所有数据
	svg_dom_destroy(g_svg_dom);
	g_svg_dom = NULL;

	if(!_tcslen(g_svg_file_name)) return;//文件名为空，返回。
	g_start = clock();
	//解析新的SVG文件
	LPCTSTR svg_content = read_svg(g_svg_file_name);
	if(svg_content == NULL) return;//svg文件的内容为空，返回。
	g_svg_dom = parse_svg(svg_content);
	free((void*)svg_content);
	g_after_parsing = clock();

	render_tree_builder = svg_render_tree_builder_create(g_svg_dom);
	g_render_tree = svg_render_tree_builder_build(render_tree_builder);
	svg_render_tree_builder_destroy(render_tree_builder);
	g_after_render_tree = clock();
	g_parsing = (float)(g_after_parsing - g_start) / CLOCKS_PER_SEC;
	g_building_render_tree = (float)(g_after_render_tree - g_after_parsing) / CLOCKS_PER_SEC;

	g_scale_ratio_index = DEFAULT_SCALE_INDEX;
	
	RESET_SCROLL_BAR(hWnd);

	REFRESH_WINDOW;
}

PRIVATE void print_status(HDC hdc){
	RECT rect = {0};
	int offset = 0;
	offset += _stprintf_s(g_status + offset, 100 - offset, _T("parse:\t%.3fs\n"), g_parsing);
	offset += _stprintf_s(g_status + offset, 100 - offset, _T("build:\t%.3fs\n"), g_building_render_tree);
	offset += _stprintf_s(g_status + offset, 100 - offset, _T("render:\t%.3fs\n"), g_rendering);
	offset += _stprintf_s(g_status + offset, 100 - offset, _T("ratio:\t%3d%%\n"), (int)(g_scale_ratios[g_scale_ratio_index] * 100));
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, g_status, _tcslen(g_status), &rect, DT_EXPANDTABS | DT_CALCRECT);//DT_EXPANDTABS是为了避免\t被缩减。
	DrawText(hdc, g_status, _tcslen(g_status), &rect, DT_EXPANDTABS);
}

PRIVATE void resize_scroll_bar(HWND hWnd){
	int scaled_view_port_width, scaled_view_port_height;

	scaled_view_port_width = (int)(g_viewport_size.Width * g_scale_ratios[g_scale_ratio_index]);
	horz_si.fMask = SIF_POS;
	GetScrollInfo(hWnd, SB_HORZ, &horz_si);
	if(g_client_area.Width < scaled_view_port_width){
		//若viewport的宽度超过了客户区的宽度，添加滚动条
		horz_si.nMax = scaled_view_port_width;
		horz_si.nPage = g_client_area.Width;
		if(horz_si.nPos + (int)horz_si.nPage > horz_si.nMax){
			horz_si.nPos = horz_si.nMax - horz_si.nPage;
		}
	}else{
		horz_si.nMax = 0;
		horz_si.nPage = 0;
	}
	horz_si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	SetScrollInfo(hWnd, SB_HORZ, &horz_si, TRUE);

	scaled_view_port_height = (int)(g_viewport_size.Height * g_scale_ratios[g_scale_ratio_index]);
	vert_si.fMask = SIF_POS;
	GetScrollInfo(hWnd, SB_VERT, &vert_si);
	if(g_client_area.Height < scaled_view_port_height){
		vert_si.nMax = scaled_view_port_height;
		vert_si.nPage = g_client_area.Height;
		if(vert_si.nPos + (int)vert_si.nPage > vert_si.nMax){
			vert_si.nPos = vert_si.nMax - vert_si.nPage;
		}
	}else{
		vert_si.nMax = 0;
		vert_si.nPage = 0;
	}
	vert_si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	SetScrollInfo(hWnd, SB_VERT, &vert_si, TRUE);
}

PRIVATE void relocate_scroll_bar(HWND hWnd){
	horz_si.nPos = 0;
	horz_si.fMask = SIF_POS;
	SetScrollInfo(hWnd, SB_HORZ, &horz_si, TRUE);
	vert_si.nPos = 0;
	vert_si.fMask = SIF_POS;
	SetScrollInfo(hWnd, SB_VERT, &vert_si, TRUE);
}