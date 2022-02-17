/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "graphics/d3d9.h"

#pragma comment(lib, "d3d9.lib")

// Singleton
static D3D9 instance;
D3D9* D3D9::GetInstance(void) { return &instance; }

void D3D9::Create (HWND window_handle, bool is_fullscreen)
{
	// Create a D3D interface object
	m_interface = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_interface == NULL) { MessageBox(NULL, "Failed to create D3D9 interface", "Fatal Error", MB_OK); exit(-1); }

	// Query the display mode of the default adapter
	HRESULT result = m_interface->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_display_mode);
	if (FAILED(result)) { MessageBox(NULL, "Failed to locate display adapter", "Fatal Error", MB_OK); exit(-1); }

	// Create a D3D display device
	m_present_parameters.BackBufferWidth        = m_display_mode.Width;
	m_present_parameters.BackBufferHeight       = m_display_mode.Height;
	m_present_parameters.BackBufferFormat       = m_display_mode.Format;
	m_present_parameters.MultiSampleType        = D3DMULTISAMPLE_NONE;
	m_present_parameters.MultiSampleQuality     = 0;
	m_present_parameters.hDeviceWindow          = window_handle;
	m_present_parameters.EnableAutoDepthStencil = true;
	m_present_parameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_present_parameters.Flags                  = 0;
	m_present_parameters.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
	if (is_fullscreen)
	{
		m_present_parameters.BackBufferCount            = 2;
		m_present_parameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
		m_present_parameters.Windowed                   = false;
		m_present_parameters.FullScreen_RefreshRateInHz = m_display_mode.RefreshRate;
	}
	else
	{
		m_present_parameters.BackBufferCount            = 1;
		m_present_parameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
		m_present_parameters.Windowed                   = true;
		m_present_parameters.FullScreen_RefreshRateInHz = 0;
	}

	result = m_interface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window_handle, D3DCREATE_HARDWARE_VERTEXPROCESSING + D3DCREATE_PUREDEVICE + D3DCREATE_MULTITHREADED, &m_present_parameters, &m_device);
	if (FAILED(result)) { MessageBox(NULL, "Failed to initialize display device", "Fatal Error", MB_OK); exit(-1); }
}


D3D9::~D3D9(void)
{
	if (m_device) m_device->Release();
	if (m_interface) m_interface->Release();
}


void D3D9::Reset(bool is_fullscreen)
{
	if (is_fullscreen)
	{
		m_present_parameters.BackBufferCount            = 2;
		m_present_parameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
		m_present_parameters.Windowed                   = false;
		m_present_parameters.FullScreen_RefreshRateInHz = m_display_mode.RefreshRate;
	}
	else
	{
		m_present_parameters.BackBufferCount            = 1;
		m_present_parameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
		m_present_parameters.Windowed                   = true;
		m_present_parameters.FullScreen_RefreshRateInHz = 0;
	}
	HRESULT result = m_device->Reset(&m_present_parameters);
	if (FAILED(result)) { MessageBox(NULL, "Failed to reset D3D9 device", "Fatal Error", MB_OK); exit(-1); }
}


bool D3D9::BeginDraw(void)
{
	m_device_status = m_device->TestCooperativeLevel();
	if (m_device_status != D3D_OK)
	{
		if (m_device_status == D3DERR_DEVICENOTRESET)
		{
			Reset(!m_present_parameters.Windowed);
		}

		// Can't render right now
		return false;
	}

	// Okay to begin drawing
	m_device->BeginScene();
	return true;
}


void D3D9::EndDraw(void)
{
	// Use status that we collected in BeginDraw()
	if (m_device_status == D3D_OK)
	{
		m_device->EndScene();
	}
}


void D3D9::Present(int client_width, int client_height)
{
	// Use status that we collected in BeginDraw()
	if (m_device_status == D3D_OK)
	{
		// Present back buffer
		if (!m_present_parameters.Windowed)
		{
			m_device->Present(NULL, NULL, NULL, NULL);
		}
		else
		{
			// Present only the client area visible in the window
			RECT r = { 0, 0, client_width, client_height };
			m_device->Present(&r, &r, NULL, NULL);
		}
	}
}


IDirect3DVertexBuffer9* D3D9::UploadVertices(void* vertices, unsigned int length, unsigned int fvf)
{
	IDirect3DVertexBuffer9* vertex_buffer;
	m_device->CreateVertexBuffer(length, D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &vertex_buffer, NULL);

	void* vertex_memory;;
	vertex_buffer->Lock(0, 0, &vertex_memory, 0);
	memcpy(vertex_memory, vertices, length);
	vertex_buffer->Unlock();
	return vertex_buffer;
}


IDirect3DIndexBuffer9* D3D9::UploadIndices(void* indices, unsigned int length)
{
	IDirect3DIndexBuffer9* index_buffer;
	m_device->CreateIndexBuffer(length, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &index_buffer, NULL);

	void* index_memory;
	index_buffer->Lock(0, 0, &index_memory, 0);
	memcpy(index_memory, indices, length);
	index_buffer->Unlock();
	return index_buffer;
}
