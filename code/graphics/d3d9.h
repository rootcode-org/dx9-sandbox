/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

#define D3D_DEBUG_INFO
#include <windows.h>
#include <d3d9.h>

class D3D9
{
public:
	~D3D9(void);

	void Create(HWND window_handle, bool is_fullscreen);
	bool BeginDraw(void);
	void EndDraw(void);
	void Present(int client_width, int client_height);
	void Reset(bool is_fullscreen);

	IDirect3DDevice9*       GetDevice(void) { return m_device; }
	IDirect3DVertexBuffer9* UploadVertices(void* vertices, unsigned int length, unsigned int fvf);
	IDirect3DIndexBuffer9*  UploadIndices(void* indices, unsigned int length);

	static D3D9* GetInstance(void);

private:
	IDirect3D9*	          m_interface = NULL;
	IDirect3DDevice9*	  m_device = NULL;
	D3DDISPLAYMODE		  m_display_mode;
	D3DPRESENT_PARAMETERS m_present_parameters;
	HRESULT				  m_device_status = D3D_OK;
};
