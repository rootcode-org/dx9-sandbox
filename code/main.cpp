/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"

#include <fstream>
#include <chrono>
#include "core/keyboard.h"
#include "core/mouse.h"
#include "core/window.h"
#include "graphics/d3d9.h"
#include "graphics/viewport.h"
#include "entity/entity.h"
#include "entity/entity_manager.h"
#include "entity/plank.h"
#include "entity/sphere.h"
#include "entity/camera.h"

#if defined(_WINDOWS)
#include <windows.h>
#endif

static Viewport viewport;
void OnWindowRedraw(void);
void OnGraphicsReset(bool is_fullscreen);

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	(nCmdShow); (hPrevInstance); (lpCmdLine);		// eliminate warning for unreferenced parameters

	// Configure memory allocators
	Heap::GetInstance()->EnableLeakTracking(true);
	Heap::GetInstance()->EnableSentinel(true);
	Heap::GetInstance()->EnableFillOnFree(true);

	// Create application window
	auto application_name = "dx9-sandbox";
	Window* window = Window::GetInstance();
	window->Create(application_name, hInstance);
	window->Show();

	// Configure rendering
	D3D9::GetInstance()->Create(window->GetHandle(), window->IsFullScreen());
	window->SetResetCallback(OnGraphicsReset);
	window->SetRedrawCallback(OnWindowRedraw);
	viewport.SetViewFrustrum(60.0f, 40.0f, 3000.0f);
	viewport.SetViewDimensions(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	Matrix m;
	m.InitWithIdentity();
	m.tz = -200;
	viewport.SetViewPlacement(m);

	// Create an object
	Plank* plank  = new Plank();

	// application main loop
	auto loop_time = std::chrono::high_resolution_clock::now();
	while (!window->IsClosing())
	{
		auto current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed_time(current_time - loop_time);
		loop_time = current_time;
		window->Update();
		EntityManager::GetInstance().UpdateAll(elapsed_time.count());
		OnWindowRedraw();
	}

	// Delete objects
	delete plank;

	// Check for leaks on the way out
	Heap::GetInstance()->ReportLeaks();
	return 0;
}


void OnWindowRedraw(void) {

	Window* window = Window::GetInstance();

	if (!window->IsHidden())
	{
		viewport.Recalculate(window->GetClientWidth(), window->GetClientHeight());

		D3D9* d3d9 = D3D9::GetInstance();
		if (d3d9->BeginDraw())
		{
			IDirect3DDevice9* device = d3d9->GetDevice();

			#if defined(MATRIX_ROW_MAJOR)
			device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&viewport.GetViewMatrix());
			device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&viewport.GetProjectionMatrix());
			#elif defined(MATRIX_COLUMN_MAJOR)
			Matrix m = viewport.GetViewMatrix();
			device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m.Transpose());
			m = viewport.GetProjectionMatrix();
			device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&m.Transpose());
			#endif

			D3DVIEWPORT9 v;
			v.X = viewport.GetViewportX();
			v.Y = viewport.GetViewportY();
			v.Width = viewport.GetViewportWidth();
			v.Height = viewport.GetViewportHeight();
			v.MinZ = viewport.GetMinZ();
			v.MaxZ = viewport.GetMaxZ();
			device->SetViewport(&v);
			device->Clear(0, NULL, D3DCLEAR_TARGET + D3DCLEAR_ZBUFFER + D3DCLEAR_STENCIL, 0x00000000, 1.0, 0);
			EntityManager::GetInstance().DrawAll(viewport);
			d3d9->EndDraw();
			d3d9->Present(window->GetClientWidth(), window->GetClientHeight());
		}
	}
}


void OnGraphicsReset(bool is_fullscreen)
{
	is_fullscreen;
	EntityManager::GetInstance().DestroyAllResources();
	D3D9::GetInstance()->Reset(is_fullscreen);
	EntityManager::GetInstance().CreateAllResources();
}
