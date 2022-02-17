/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include <tchar.h>
#include <windows.h>
#include "core/keyboard.h"
#include "core/mouse.h"

class Window {
public:
	~Window(void);

	void Create(const char* window_name, HINSTANCE instance);
	void Show(void);
	void Update(void);
	void ToggleFullScreen(void);
	void SetDelayCallback(void (*callback) (void))  { m_delay_callback = callback; }
	void SetResetCallback(void (*callback) (bool))  { m_reset_callback = callback; }
	void SetRedrawCallback(void (*callback) (void)) { m_redraw_callback = callback; }

	HWND  GetHandle(void)       { return m_window_handle; }
	int   GetClientWidth(void)  { return m_is_full_screen ? m_client_max_width : m_client_width; }
	int   GetClientHeight(void) { return m_is_full_screen ? m_client_max_height : m_client_height; }
	bool  IsFullScreen(void)    { return m_is_full_screen; }
	bool  IsClosing(void)       { return m_is_closing; }
	bool  IsHidden(void);

	LRESULT MessageHandler(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam);

	static Window* GetInstance(void);

private:
	// Configuration variables
	const int     m_window_style = WS_CAPTION + WS_MAXIMIZEBOX + WS_MINIMIZEBOX + WS_SIZEBOX + WS_SYSMENU;
	const _TCHAR* m_window_name  = NULL;

	// System handles
	HINSTANCE m_application_instance = NULL;
	HWND      m_window_handle        = NULL;

	// Window frame metrics
	int m_border_width;
	int m_border_height;
	int m_caption_height;
	int m_menu_height;
	int m_frame_width;			// left border + right border
	int m_frame_height;			// top border + caption + menu + bottom border

	// Window state
	bool m_is_full_screen;
	bool m_is_active  = true;
	bool m_is_closing = false;
	WINDOWPLACEMENT m_saved_placement;

	// Client area
	int m_client_width;
	int m_client_height;
	int m_client_min_width;
	int m_client_min_height;
	int m_client_max_width;
	int m_client_max_height;

	void (*m_delay_callback)  (void) = NULL;		// executed when window functions cause a delay
	void (*m_reset_callback)  (bool) = NULL;		// executed when the graphics system should reset
	void (*m_redraw_callback) (void) = NULL;		// executed when the whole client area should be redrawn
};
