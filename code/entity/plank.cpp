/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "core/keyboard.h"
#include "math/vector.h"
#include "graphics/d3d9.h"
#include "graphics/viewport.h"
#include "entity/plank.h"

#define USE_RHW 0

struct PlankVertex    { float x, y, z;    unsigned int color; };
struct PlankVertexRHW { float x, y, z, w; unsigned int color; };

static PlankVertex plank_vertices[] = {

	// plank vertices
	{-1.0f,  1.0f, -1.0f, 0x00ff8000},
	{ 1.0f,  1.0f, -1.0f, 0x00ff8000},
	{-1.0f, -1.0f, -1.0f, 0x0000ff00},
	{ 1.0f, -1.0f, -1.0f, 0x0000ff00},
	{-1.0f, -1.0f,  1.0f, 0x00ff8000},
	{ 1.0f, -1.0f,  1.0f, 0x00ff8000},
	{-1.0f,  1.0f,  1.0f, 0x0000ff00},
	{ 1.0f,  1.0f,  1.0f, 0x0000ff00},

	// axis vertices
	{-1.0f,    0,     0,  0xffff0000},
	{ 1.0f,    0,     0,  0xffff0000},
	{   0,  -1.0f,    0,  0xff00ff00},
	{   0,   1.0f,    0,  0xff00ff00},
	{   0,     0,  -1.0f, 0xff0000ff},
	{   0,     0,   1.0f, 0xff0000ff}
};

static short plank_indices[] = {
	0,1,2,3,4,5,6,7,	// front/top/back
	7,5,				// degenerate faces
	5,3,7,1,6,0,4,2		// right/bottom/left
};

static const float plank_x_scale = 15.0f;
static const float plank_y_scale = 05.0f;
static const float plank_z_scale = 25.0f;
static const float axis_x_scale  = 40.0f;
static const float axis_y_scale  = 35.0f;
static const float axis_z_scale  = 50.0f;

Plank::Plank(void)
{
	m_radius = 25;
	m_matrix.InitWithIdentity();
	CreateResources();
}


Plank::~Plank(void)
{
	DestroyResources();
}


void Plank::Update(float frame_time)
{
	float turn_rate = frame_time * 60 * 0.012f;
	float heading_rate = 0;
	float pitch_rate = 0;
	float roll_rate = 0;
	Keyboard* k = Keyboard::GetInstance();
	if (k->IsDown(0x41)) heading_rate = -turn_rate;		// A
	if (k->IsDown(0x44)) heading_rate = turn_rate;		// D
	if (k->IsDown(0x57)) pitch_rate = -turn_rate;		// W
	if (k->IsDown(0x53)) pitch_rate = turn_rate;		// S
	if (k->IsDown(0x45)) roll_rate = -turn_rate;		// E
	if (k->IsDown(0x51)) roll_rate = turn_rate;		// Q
	if (k->IsDown(0x52)) { m_matrix.InitWithIdentity(); }	// R

	Matrix change;
	change.InitWithExtrinsicZXY(roll_rate, pitch_rate, heading_rate);

#if defined(MATRIX_ROW_MAJOR)
	m_matrix.PreMultiply(change);
#elif defined(MATRIX_COLUMN_MAJOR)
	m_matrix *= change;
#endif
}


const unsigned plank_fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;
const unsigned plank_fvf_rhw = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
static IDirect3DVertexBuffer9* plank_vertex_buffer = NULL;
static IDirect3DVertexBuffer9* plank_vertex_rhw_buffer = NULL;
static IDirect3DIndexBuffer9* plank_index_buffer = NULL;
static IDirect3DStateBlock9* plank_state_block = NULL;
static int plank_ref_count = 0;

void Plank::CreateResources(void)
{
	if (plank_ref_count == 0)
	{
		D3D9* d3d9 = D3D9::GetInstance();
		plank_vertex_buffer = d3d9->UploadVertices(plank_vertices, sizeof(plank_vertices), plank_fvf);
		plank_index_buffer = d3d9->UploadIndices(plank_indices, sizeof(plank_indices));

		// Create buffer for pre-transformed vertices
		unsigned int num_vertices = sizeof(plank_vertices) / sizeof(PlankVertex);
		IDirect3DDevice9* device = d3d9->GetDevice();
		device->CreateVertexBuffer(num_vertices * sizeof(PlankVertexRHW), D3DUSAGE_WRITEONLY, plank_fvf_rhw, D3DPOOL_DEFAULT, &plank_vertex_rhw_buffer, NULL);
	}
	plank_ref_count++;
}


void Plank::DestroyResources(void)
{
	plank_ref_count--;
	if (plank_ref_count == 0)
	{
		if (plank_vertex_buffer) { plank_vertex_buffer->Release();         plank_vertex_buffer = NULL; }
		if (plank_vertex_rhw_buffer) { plank_vertex_rhw_buffer->Release(); plank_vertex_rhw_buffer = NULL; }
		if (plank_index_buffer) { plank_index_buffer->Release();           plank_index_buffer = NULL; }
		if (plank_state_block) { plank_state_block->Release();             plank_state_block = NULL; }
	}
}


void Plank::Draw(Viewport& viewport)
{
	Vector position = m_matrix.GetTranslationVector();
	float distance = viewport.GetDistanceFromNearClipPlane(position, m_radius);
	if (distance == 0.0) return;

	IDirect3DDevice9* device = D3D9::GetInstance()->GetDevice();

	// Set render state
	if (plank_state_block)
	{
		plank_state_block->Apply();
	}
	else
	{
		device->BeginStateBlock();
#if USE_RHW
		device->SetStreamSource(0, plank_vertex_rhw_buffer, 0, sizeof(PlankVertexRHW));
		device->SetFVF(plank_fvf_rhw);
#else
		device->SetStreamSource(0, plank_vertex_buffer, 0, sizeof(PlankVertex));
		device->SetFVF(plank_fvf);
#endif
		device->SetIndices(plank_index_buffer);
		device->SetRenderState(D3DRS_LIGHTING, false);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		device->SetRenderState(D3DRS_ZWRITEENABLE, true);
		device->EndStateBlock(&plank_state_block);
	}

	// Generate world matrices
	Matrix plank_matrix = m_matrix;
	plank_matrix.PreScaleX(plank_x_scale);
	plank_matrix.PreScaleY(plank_y_scale);
	plank_matrix.PreScaleZ(plank_z_scale);
	Matrix axis_matrix = m_matrix;
	axis_matrix.PreScaleX(axis_x_scale);
	axis_matrix.PreScaleY(axis_y_scale);
	axis_matrix.PreScaleZ(axis_z_scale);

	// Transform vertices
#if USE_RHW
#if defined(MATRIX_ROW_MAJOR)
	plank_matrix *= viewport.GetTransformMatrix();
	axis_matrix *= viewport.GetTransformMatrix();
#elif defined(MATRIX_COLUMN_MAJOR)
	plank_matrix.PreMultiply(viewport.GetTransformMatrix());
	axis_matrix.PreMultiply(viewport.GetTransformMatrix());
#endif
	void* vertex_memory;
	plank_vertex_rhw_buffer->Lock(0, 0, &vertex_memory, 0);
	Vertex::Transform((Vertex::Vertex4C*) vertex_memory, (Vertex::Vertex3C*) plank_vertices, plank_matrix, 8);
	Vertex::Transform(((Vertex::Vertex4C*) vertex_memory) + 8, ((Vertex::Vertex3C*) plank_vertices) + 8, axis_matrix, 6);
	plank_vertex_rhw_buffer->Unlock();
#endif

	// Draw plank
#if !USE_RHW
#if defined(MATRIX_COLUMN_MAJOR)
	plank_matrix.Transpose();
#endif
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&plank_matrix);
#endif
	device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 18, 0, 16);

	// Draw axes
#if !USE_RHW
#if defined(MATRIX_COLUMN_MAJOR)
	axis_matrix.Transpose();
#endif
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&axis_matrix);
#endif
	device->DrawPrimitive(D3DPT_LINESTRIP, 8, 1);
	device->DrawPrimitive(D3DPT_LINESTRIP, 10, 1);
	device->DrawPrimitive(D3DPT_LINESTRIP, 12, 1);
}
