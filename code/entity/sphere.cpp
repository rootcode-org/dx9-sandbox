/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "graphics/d3d9.h"
#include "graphics/viewport.h"
#include "entity/sphere.h"

struct SphereVertex    {float x, y, z; unsigned color;};
struct SpherePrimitive {D3DPRIMITIVETYPE primitive_type; unsigned int start_vertex; unsigned int num_faces;};
const unsigned sphere_fvf = D3DFVF_XYZ|D3DFVF_DIFFUSE;

static IDirect3DVertexBuffer9* sphere_vertex_buffer_16 = NULL;
static IDirect3DIndexBuffer9*  sphere_index_buffer_16  = NULL;
static int sphere_num_vertices_16 = 0;
static int sphere_num_faces_16    = 0;

static IDirect3DVertexBuffer9* sphere_vertex_buffer_8 = NULL;
static IDirect3DIndexBuffer9*  sphere_index_buffer_8  = NULL;
static int sphere_num_vertices_8 = 0;
static int sphere_num_faces_8 = 0;

static IDirect3DVertexBuffer9* sphere_vertex_buffer_4 = NULL;
static IDirect3DIndexBuffer9*  sphere_index_buffer_4  = NULL;
static int sphere_num_vertices_4 = 0;
static int sphere_num_faces_4 = 0;

static IDirect3DStateBlock9* sphere_state_block = NULL;
static int sphere_ref_count = 0;

// Private functions
static void _generateSphereMesh (float radius, int steps, IDirect3DVertexBuffer9** ppv, IDirect3DIndexBuffer9** ppi, int* nv, int* nf);

// Private data
static unsigned int color_lookup[8] = {
	0xffffffff,
	0xffff0000,
	0xffffffff,
	0xff00ff00,
	0xffffffff,
	0xff0000ff,
	0xffffffff,
	0xffff00ff
};

Sphere::Sphere(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_radius = 25.0f;
	m_spin = 0.0f;
	m_matrix.InitWithIdentity();
	CreateResources();
}


Sphere::~Sphere(void)
{
	DestroyResources();
}


void Sphere::CreateResources (void)
{
	if (sphere_ref_count == 0)
	{
		// Generate 3 levels of detail
		_generateSphereMesh(25, 4, &sphere_vertex_buffer_4, &sphere_index_buffer_4, &sphere_num_vertices_4, &sphere_num_faces_4);
		_generateSphereMesh(25, 8, &sphere_vertex_buffer_8, &sphere_index_buffer_8, &sphere_num_vertices_8, &sphere_num_faces_8);
		_generateSphereMesh(25, 16, &sphere_vertex_buffer_16, &sphere_index_buffer_16, &sphere_num_vertices_16, &sphere_num_faces_16);
	}
	sphere_ref_count++;
}


void Sphere::DestroyResources (void)
{
	sphere_ref_count--;
	if (sphere_ref_count == 0)
	{
		if (sphere_vertex_buffer_4)  sphere_vertex_buffer_4->Release();
		if (sphere_index_buffer_4)   sphere_index_buffer_4->Release();
		if (sphere_vertex_buffer_8)  sphere_vertex_buffer_8->Release();
		if (sphere_index_buffer_8)   sphere_index_buffer_8->Release();
		if (sphere_vertex_buffer_16) sphere_vertex_buffer_16->Release();
		if (sphere_index_buffer_16)  sphere_index_buffer_16->Release();
		if (sphere_state_block)      sphere_state_block->Release();
		sphere_vertex_buffer_4 = sphere_vertex_buffer_8 = sphere_vertex_buffer_16 = NULL;
		sphere_index_buffer_4  = sphere_index_buffer_8  = sphere_index_buffer_16  = NULL;
		sphere_state_block = NULL;
	}
}


void Sphere::Update (float frame_time)
{
	m_spin += frame_time * 60 * 0.012f;
	m_matrix.InitWithYRotation(m_spin);
	m_matrix.tx = m_x;
	m_matrix.ty = m_y;
	m_matrix.tz = m_z;
}


void Sphere::Draw (Viewport& viewport)
{
	Vector position = m_matrix.GetTranslationVector();
	float distance = viewport.GetDistanceFromNearClipPlane(position, m_radius);
	if (distance == 0.0) return;

	IDirect3DDevice9* device = D3D9::GetInstance()->GetDevice();
	if (sphere_state_block)
	{
		sphere_state_block->Apply();
	}
	else
	{
		device->BeginStateBlock();
		device->SetFVF(sphere_fvf);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		device->SetRenderState(D3DRS_LIGHTING, false);
		device->SetRenderState(D3DRS_ZWRITEENABLE, true);
		device->EndStateBlock(&sphere_state_block);
	}

	// Select LOD
	IDirect3DVertexBuffer9* pv;
	IDirect3DIndexBuffer9* pi;
	unsigned num_vertices;
	unsigned num_faces;
	if (distance > 2000)
	{
		pv = sphere_vertex_buffer_4;
		pi = sphere_index_buffer_4;
		num_vertices = (unsigned)sphere_num_vertices_4;
		num_faces = (unsigned)sphere_num_faces_4;
	}
	else if (distance > 1000)
	{
		pv = sphere_vertex_buffer_8;
		pi = sphere_index_buffer_8;
		num_vertices = (unsigned)sphere_num_vertices_8;
		num_faces = (unsigned)sphere_num_faces_8;
	}
	else
	{
		pv = sphere_vertex_buffer_16;
		pi = sphere_index_buffer_16;
		num_vertices = (unsigned)sphere_num_vertices_16;
		num_faces = (unsigned)sphere_num_faces_16;
	}
	device->SetStreamSource(0, pv, 0, sizeof(SphereVertex));
	device->SetIndices(pi);

	// If object is within a radius of being clipped then make it translucent
	if (distance < m_radius)
	{
		float f = distance / m_radius;
		unsigned alpha = (unsigned)(f * 255);
		unsigned blend = alpha + (alpha << 8) + (alpha << 16);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
		device->SetRenderState(D3DRS_BLENDFACTOR, blend);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	}

	// Draw sphere
#if defined(MATRIX_ROW_MAJOR)
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m_matrix);
#elif defined(MATRIX_COLUMN_MAJOR)
	Matrix m = m_matrix;
	m.Transpose();
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&m);
#endif


	device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, num_vertices, 0, num_faces);
}


static void _generateSphereMesh (float radius, int steps, IDirect3DVertexBuffer9** ppv, IDirect3DIndexBuffer9** ppi, int* nv, int* nf)
{
	int sphere_num_vertices       = (steps*(steps-1)) + 2;
	int sphere_num_faces          = ((((steps-2)*2)+2) * steps) + (steps-1);	// includes degenerates
	int sphere_num_indices        = sphere_num_faces + 2;
	SphereVertex* sphere_vertices = new SphereVertex[(unsigned)sphere_num_vertices];
	SphereVertex* next_vertex     = sphere_vertices;
	short* sphere_indices         = new short[(unsigned)sphere_num_indices];
	short* next_index             = sphere_indices;

	// first vertex is the north pole
	next_vertex->x = 0;
	next_vertex->y = radius;
	next_vertex->z = 0;
	next_vertex->color = 0x00ffffff;
	next_vertex++;

	// second vertex is the south pole
	next_vertex->x = 0;
	next_vertex->y = -radius;
	next_vertex->z = 0;
	next_vertex->color = 0x00ffffff;
	next_vertex++;

	// generate segment vertices
	float increment = (1.0f/(float) steps) * 3.141592654f;
	for (int i = 0; i < steps; i++)
	{
		for (int j = 1; j < steps; j++)
		{
			float theta = -(i * 2 * increment);
			float phi = j * increment;
			next_vertex->x = radius * cosf(theta) * sinf(phi);
			next_vertex->y = radius * cosf(phi);
			next_vertex->z = radius * sinf(theta) * sinf(phi);
			next_vertex->color = color_lookup[i % _countof(color_lookup)];
			next_vertex++;
		}
	}

	// Generate the indices
	int stride = steps-1;
	int pairs  = steps-2;
	int npole = 0;
	int spole = 1;
	int n     = 2;
	*next_index++ = (short) npole;
	*next_index++ = (short) n;

	bool wrap = false;
	for (int k = 0; k < (steps/2); k++)
	{
		if (k == (steps/2)-1) wrap = true;		// vertices will wrap on last segment

		for (int i = 0; i < pairs; i++)
		{
			*next_index++ = (short) (n+stride);
			*next_index++ = (short) (++n);
		}
		n += stride;
		*next_index++ = (short) n;
		*next_index++ = (short) spole;
		*next_index++ = (short) n;				// degenerate

		for (int j = 0; j < pairs; j++)
		{
			if (wrap) *next_index++ = (short) (n+stride-(steps*(steps-1)));
			else      *next_index++ = (short) (n+stride);
			*next_index++ = (short) (--n);
		}
		n += stride;
		if (wrap) *next_index++ = (short) (n-(steps*(steps-1)));
		else      *next_index++ = (short) n;
		*next_index++ = (short) npole;

		if (!wrap) *next_index++ = (short) n;	// degenerate
	}

	D3D9* d3d9 = D3D9::GetInstance();
	IDirect3DVertexBuffer9* sphere_vertex_buffer = d3d9->UploadVertices(sphere_vertices, sphere_num_vertices * sizeof(SphereVertex), sphere_fvf);
	IDirect3DIndexBuffer9* sphere_index_buffer = d3d9->UploadIndices(sphere_indices, sphere_num_indices * sizeof(short));
	delete[] sphere_vertices;
	delete[] sphere_indices;

	*ppv = sphere_vertex_buffer;
	*ppi = sphere_index_buffer;
	*nv  = sphere_num_vertices;
	*nf  = sphere_num_faces;
}
