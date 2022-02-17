/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <malloc.h>
#include <cstdlib>
#include "core/window.h"
#include "core/keyboard.h"
#include "core/mouse.h"

// Singleton
static Window winstance;
Window* Window::GetInstance() { return &winstance; }


static LRESULT CALLBACK _WindowProc(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Window::GetInstance()->MessageHandler(window_handle, message, wParam, lParam);
}


void Window::Create (const char* window_name, HINSTANCE instance)
{
	m_window_name          = window_name;
	m_application_instance = instance;

#if defined(_DEBUG)
	m_is_full_screen = false;
#else
	m_is_full_screen = true;
#endif

#if !defined(_DEBUG)
	// Terminate if there an instance of this application already running
	CreateMutex (NULL, true, m_window_name);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Find the window handle of the existing instance and bring that window to the foreground
		HWND other_handle = FindWindow (m_window_name, m_window_name);
		if (other_handle != NULL)
		{
			SetForegroundWindow (other_handle);
		}

		// Terminate this application instance
		exit(0);
	}
#endif	// _DEBUG

	// Get the dimensions of the system display and the window frame elements
	int system_width  = GetSystemMetrics (SM_CXSCREEN);
	int system_height = GetSystemMetrics (SM_CYSCREEN);
	m_border_width    = GetSystemMetrics (SM_CXSIZEFRAME);
	m_border_height   = GetSystemMetrics (SM_CYSIZEFRAME);
	m_caption_height  = GetSystemMetrics (SM_CYCAPTION);
	m_menu_height     = GetSystemMetrics (SM_CYMENU);
	m_frame_width     = m_border_width  * 2;
	m_frame_height    = m_border_height * 2 + m_menu_height + m_caption_height;

	// Calculate the minimum and maximum dimensions of the window client area
	m_client_max_width  = system_width;
	m_client_max_height = system_height;

	float aspect_ratio  = (float)m_client_max_width / (float)m_client_max_height;
	m_client_min_width  = 160;
	m_client_min_height = (int) (m_client_min_width / aspect_ratio);

	// Set the default window dimensions
	m_client_width  = system_width/2;
	m_client_height = (int) (m_client_width / aspect_ratio);
	int window_x  = (system_width  - m_client_width  - m_frame_width)  / 2;
	int window_y  = (system_height - m_client_height - m_frame_height) / 2;

	// Register the window class for the main window.
	WNDCLASSEX wcex;
	wcex.cbSize		   = sizeof(WNDCLASSEX); 
	wcex.style		   = 0;
	wcex.lpfnWndProc   = (WNDPROC)_WindowProc;
	wcex.cbClsExtra	   = 0;
	wcex.cbWndExtra	   = 0;
	wcex.hInstance	   = m_application_instance;
	wcex.hIcon		   = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor	   = LoadCursor (NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.hIconSm       = NULL;
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = window_name;
	ATOM classatom     = RegisterClassEx(&wcex);
	if (!classatom) { MessageBox(NULL, "Unable to register window class", "Fatal Error", MB_OK); exit(-1); }

	// Create the main application window.
	DWORD style;
	if (m_is_full_screen) style = WS_POPUP;			// No caption or borders
	else                  style = m_window_style;	// Includes caption, system menu, sizing borders, and minimize/maximize/close buttons
	m_window_handle = CreateWindowEx(
		0,									// exStyle
		m_window_name,						// Class name
		m_window_name,						// Window name
		style,								// Style
		window_x,							// X position
		window_y,							// Y position
		m_client_width  + m_frame_width,	// Width
		m_client_height + m_frame_height,	// Height
		NULL,								// Parent window
		NULL,								// Menu
		m_application_instance,				// Application instance handle
		NULL								// Parameter
	);					

	// If we couldn't create the window then just forget the whole thing and terminate
	if (!m_window_handle) { MessageBox(NULL, "Unable to create application window", "Fatal Error", MB_OK); exit(-1); }

	// Initialize raw input for keyboard and mouse
	RAWINPUTDEVICE raw_input_devices[2];
	raw_input_devices[0].usUsagePage = 0x01;
	raw_input_devices[0].usUsage = 0x06;				// keyboard
	raw_input_devices[0].dwFlags = 0;
	raw_input_devices[0].hwndTarget = m_window_handle;
	raw_input_devices[1].usUsagePage = 0x01;
	raw_input_devices[1].usUsage = 0x02;				// mouse
	raw_input_devices[1].dwFlags = 0;
	raw_input_devices[1].hwndTarget = m_window_handle;
	RegisterRawInputDevices(raw_input_devices, 2, sizeof(RAWINPUTDEVICE));

	// Save a copy of the current window position. If we started in fullscreen mode, we need this
	// saved position if the user toggles back to windowed mode.
	m_saved_placement.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement (m_window_handle, &m_saved_placement);

	// If we are starting in fullscreen mode go ahead and set the window size to cover the entire display
	if (m_is_full_screen)
	{
		SetWindowPos(m_window_handle, HWND_NOTOPMOST, -m_border_width, -m_border_width - m_menu_height - m_caption_height,
			m_client_max_width + m_frame_width, m_client_max_height + m_frame_height, 0);
	}
}


Window::~Window (void)
{
	DestroyWindow(m_window_handle);
	UnregisterClass(m_window_name, m_application_instance);		// We used the window name as the class name
}


void Window::Update (void)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&msg);
	}
}


void Window::Show (void)
{
	ShowWindow (m_window_handle, SW_SHOWNORMAL);
}


void Window::ToggleFullScreen (void)
{
	m_is_full_screen = !m_is_full_screen;

	if (m_is_full_screen)
	{
		// Save the window position
		m_saved_placement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement (m_window_handle, &m_saved_placement);

		// Resize the window for full screen
		SetWindowPos(m_window_handle, HWND_NOTOPMOST, -m_border_width, -m_border_width - m_menu_height - m_caption_height,
			m_client_max_width + m_frame_width, m_client_max_height + m_frame_height,
			SWP_NOREDRAW + SWP_NOCOPYBITS + SWP_DEFERERASE);

		// Redraw at new window size before resetting; this reduces visual artifacts
		if (m_redraw_callback) m_redraw_callback();

		// Reset the graphics system for fullscreen mode
		if (m_reset_callback) m_reset_callback(true);
	}
	else
	{
		// Reset graphics for windowed mode and restore window position
		if (m_reset_callback) m_reset_callback(false);
		SetWindowLong(m_window_handle, GWL_STYLE, m_window_style);
		SetWindowPlacement(m_window_handle, &m_saved_placement);
	}
}


bool Window::IsHidden (void)
{
	if (((m_is_full_screen) && (!m_is_active)) || (IsIconic (m_window_handle))) return true;
	else																	    return false;
}


LRESULT Window::MessageHandler(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	// If Alt-Enter is pressed then toggle between full-screen and windowed modes
	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN)
		{
			ToggleFullScreen();
		}
		break;


	// The WM_SIZING message is received when the user is manually resizing by dragging the window frame.
	// We respond to this message by enforcing the current aspect ratio and limiting the client area size
	// to minimum and maximum sizes.
	case WM_SIZING:
		{
			// Get the new dimensions of the client area
			RECT *r = (RECT*) lParam;
			int width  = r->right  - r->left - m_frame_width;
			int height = r->bottom - r->top  - m_frame_height;

			// Limit the client area to minimum and maximum sizes
			if (width  < m_client_min_width)  width  = m_client_min_width;
			if (width  > m_client_max_width)  width  = m_client_max_width;
			if (height < m_client_min_height) height = m_client_min_height;
			if (height > m_client_max_height) height = m_client_max_height;

			// Ensure the new client area has a fixed aspect ratio, and adjust the window size and position
			// depending on which edge was dragged. The window position needs adjustment in some cases to
			// prevent the window from sliding when the minimum or maximum size is reached.
			float aspect_ratio = (float)m_client_max_width / (float)m_client_max_height;
			int width_from_height = (int) (height * aspect_ratio);
			int height_from_width = (int) (width  / aspect_ratio);
			switch (wParam)
			{
			case WMSZ_TOP:
				width  = width_from_height;
				r->top = r->bottom - height - m_frame_height;
				break;
			case WMSZ_BOTTOM:
				width  = width_from_height;
				break;
			case WMSZ_LEFT:
				height  = height_from_width;
				r->left = r->right - width - m_frame_width;
				break;
			case WMSZ_RIGHT:
				height = height_from_width;
				break;
			case WMSZ_TOPLEFT:
				if (width < width_from_height) width  = width_from_height;
				else						   height = height_from_width;
				r->left = r->right  - width  - m_frame_width;
				r->top  = r->bottom - height - m_frame_height;
				if (r->top < -(m_menu_height-1) )
				{
					r->top  = -(m_menu_height-1);
					height  = r->bottom - m_frame_height + m_menu_height - 1;
					width   = (int) (height * aspect_ratio);
					r->left = r->right  - width - m_frame_width;
				}
				break;
			case WMSZ_TOPRIGHT:
				if (width < width_from_height) width  = width_from_height;
				else						   height = height_from_width;
				r->top = r->bottom - height - m_frame_height;
				if (r->top < -(m_menu_height-1) )
				{
					r->top = -(m_menu_height-1);
					height = r->bottom - m_frame_height + m_menu_height - 1;
					width  = (int) (height * aspect_ratio);
				}
				break;
			case WMSZ_BOTTOMLEFT:
				if (width < width_from_height) width  = width_from_height;
				else						   height = height_from_width;
				r->left = r->right - width - m_frame_width;
				break;
			case WMSZ_BOTTOMRIGHT:
				if (width < width_from_height) width  = width_from_height;
				else						   height = height_from_width;
				break;
			}

			// Return the revised position and size of the window
			r->right  = r->left + width  + m_frame_width;
			r->bottom = r->top  + height + m_frame_height;
		}
		return true;


	// The WM_SIZE message is received when the window size is changed.
	// We record the new client area size and force a full redraw of the client area.
	case WM_SIZE:
		{
			int width  = (int) (lParam & 0xffff);
			int height = (int) (lParam >> 16);

			// When switching to fullscreen mode, Windows will mistakenly pass the size of the window
			// instead of the client area. Hence we must limit the size to the maximum client area.
			if (width  > m_client_max_width)  width  = m_client_max_width;
			if (height > m_client_max_height) height = m_client_max_height;

			switch (wParam)
			{
			case SIZE_MAXHIDE:				// Another window has been maximized.
			case SIZE_MINIMIZED:			// This window has been minimized.
				break;
			case SIZE_RESTORED:				// This window has been resized, but not maximized or minimized.
			case SIZE_MAXIMIZED:			// This window has been maximized.
			case SIZE_MAXSHOW:				// Another window has been restored, having previously been maximized.
				m_client_width  = width;
				m_client_height = height;
				PostMessage (window_handle, WM_PAINT, 0, 0);
				break;
			}
		}
		return 0;


	// The WM_MINMAXINFO message is received when the window size or position is about to change. It queries
	// the application for the minimum and maximum window sizes allowed. We use it to ensure that a maximized
	// window does not exceed the maximum allowed client area.
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* info = (MINMAXINFO*) lParam;
			info->ptMaxSize.x = m_client_max_width  + m_frame_width;
			info->ptMaxSize.y = m_client_max_height + m_frame_height;
		}
		return 0;

	// The WM_PAINT message is sent whenever some or all of the window client must be redrawn.
	// This can occur when the window is moved, resized, or unobscured by another window.
	case WM_PAINT:
		if (m_redraw_callback) m_redraw_callback();
		ValidateRect(window_handle, NULL);
		return 0;

	// The WM_ACTIVATEAPP message is received when the application is activated or de-activated.
	// We simply keep track of the current state.
	case WM_ACTIVATEAPP:
		if (wParam) m_is_active = true;
		else		m_is_active = false;
		return 0;

	// The WM_ACTIVATE message is received when the window is activated or deactivated. This can happen for many reasons,
	// one of which is a dialog box being created or destroyed. If the window is activated we relatch the game timer to
	// maintain contiguous game time. If the window is deactivated in fullscreen mode, we minimize it in order to ensure
	// that dialog boxes display correctly (i.e. are not hidden behind the fullscreen window).
	case WM_ACTIVATE:
		switch (wParam)
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if (m_delay_callback) m_delay_callback();
			break;
		case WA_INACTIVE:
			if (m_is_full_screen) ShowWindow(window_handle, SW_MINIMIZE);
			break;
		}
		return 0;

	// The WM_EXITSIZEMOVE message is received whenever the user finishes moving or sizing the window
	// and control is returned to the application.
	case WM_EXITSIZEMOVE:
		if (m_delay_callback) m_delay_callback();
		return 0;

	// The WM_CAPTURECHANGED message is received when a window is losing the mouse capture. We use it
	// to detect when the user selects and holds any of the minimize, maximize or exit icons on the
	// system menu, but moves the cursor off the icon before releasing the button (thereby not selecting
	// them). Why this message is generated for doing that I have no idea.
	case WM_CAPTURECHANGED:
		if (m_delay_callback) m_delay_callback();
		break;

	// The WM_ERASEBKGND message is received to signal the application to erase it's background. We
	// do nothing here, but return a non-zero value to say we took care of it.
	case WM_ERASEBKGND:
		return 1;

	// The WM_INPUT message supplies raw input from the keyboard and mouse
	case WM_INPUT:
		{
			// query size of raw input data
			unsigned int ri_size;
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &ri_size, sizeof(RAWINPUTHEADER));

			// get raw input data
			RAWINPUT* ri = (RAWINPUT*)_malloca(ri_size);
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, ri, &ri_size, sizeof(RAWINPUTHEADER));

			// parse raw input
			if (ri->header.dwType == RIM_TYPEKEYBOARD)
			{
				// see https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawkeyboard
				RAWKEYBOARD* data = (RAWKEYBOARD*) (&ri->data);
				Keyboard::GetInstance()->RawInput(data->VKey, (data->Flags & 0x01) ^ 0x01);
			}
			else if (ri->header.dwType == RIM_TYPEMOUSE)
			{
				// see https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawmouse
				RAWMOUSE* data = (RAWMOUSE*)(&ri->data);
				int wheel = 0;
				if (data->usButtonFlags & RI_MOUSE_WHEEL)
				{
					wheel = data->usButtonData;
				}
				Mouse::GetInstance()->RawInput(data->lLastX, data->lLastX, wheel, data->ulRawButtons);
			}
		}
		return 0;

	// The WM_CLOSE message is received whenever the application is instructed to close
	case WM_CLOSE:
		m_is_closing = true;
		return 0;
   }

	// Use the default message handler for anything not handled above
	return DefWindowProc(window_handle, message, wParam, lParam);
}
