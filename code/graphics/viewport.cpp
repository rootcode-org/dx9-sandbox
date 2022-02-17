/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "graphics/viewport.h"

Viewport::Viewport (void)
{
	// Set some reasonable defaults
	m_horizontal_fov = 60.0f * 3.141592654f / 180.0f;	// Horizontal Field of View in degrees (vertical field of view is calculated automatically from the aspect ratio)
	m_near_clip_z    = 40.0f;							// Z position of near clip plane
	m_far_clip_z     = 3000.0f;							// Z position of far clip plane
	m_view_x         = 0.0f;							// X position of viewport window on display (0.0 to 1.0)
	m_view_y         = 0.0f;							// Y position of viewport window on display (0.0 to 1.0)
	m_view_width     = 1.0f;							// Width of viewport window on display (0.0 to 1.0)
	m_view_height    = 1.0f;							// Height of viewport window on display (0.0 to 1.0)
	m_min_z          = 0.0f;							// Vertex Z is scaled from minZ to maxZ
	m_max_z          = 1.0f;							// "
	m_view_matrix.InitWithIdentity();					// View position and orientation
}


void Viewport::SetViewPlacement(const Matrix& m)
{
	m_view_matrix = m;
	m_view_position.x = m_view_matrix.tx;
	m_view_position.y = m_view_matrix.ty;
	m_view_position.z = m_view_matrix.tz;
	m_view_matrix.InvertOrthogonal();
}


void Viewport::SetViewFrustrum (float horizontal_fov, float near_clip_z, float far_clip_z)
{
	m_horizontal_fov = horizontal_fov * 3.141592654f / 180.0f;
	m_near_clip_z    = near_clip_z;
	m_far_clip_z     = far_clip_z;
}


void Viewport::SetViewDimensions (float x, float y, float width, float height, float min_z, float max_z)
{
	m_view_x      = x;
	m_view_y      = y;
	m_view_width  = width;
	m_view_height = height;
	m_min_z       = min_z;
	m_max_z       = max_z;
}


void Viewport::Recalculate (int display_width, int display_height)
{
	m_display_width = (float) display_width;
	m_display_height = (float) display_height;

	// Create projection matrix (transforms view space to clip space)
	//  _                    _
	//  | h 0 0            0 |
	//  | 0 v 0            0 |
	//  | 0 0 f/(f-n)      1 |
	//  | 0 0 -(f*n)/(f-n) 0 |
	//  ¯                    ¯
	// where, h = Horizontal Focal Length
	//        v = Vertical Focal Length
	//        f = Far Clip Z
	//        n = Near Clip Z
	//
	m_projection_matrix.InitWithIdentity();
	float display_aspect = m_display_width / m_display_height;
	float view_aspect    = m_view_width / m_view_height;
	m_projection_matrix.rx = 1 / tanf (m_horizontal_fov / 2);
	m_projection_matrix.uy = m_projection_matrix.rx * display_aspect * view_aspect;
	m_projection_matrix.az = m_far_clip_z / (m_far_clip_z - m_near_clip_z);
	m_projection_matrix.tz = - (m_far_clip_z * m_near_clip_z) / (m_far_clip_z - m_near_clip_z);
	m_projection_matrix.aw = 1;
	m_projection_matrix.tw = 0;

	// Create screen matrix (transforms clip space to screen space)
	//  _                     _   
	//  | sx  0  0          0 |   
	//  | 0  -sy 0          0 | 
	//  | 0   0  maxZ-minZ  0 |   
	//  | tx  ty minZ       1 |   
	//  ¯                     ¯   
	// where, sx = Scale X to screen
	//        sy = Scale Y to screen
	//        tx = Translate X to screen
	//        ty = Translate Y to screen
	//        minZ, maxZ = Maps vertex Z into range minZ to maxZ
	//
	m_screen_matrix.InitWithIdentity();
	m_screen_matrix.rx = m_view_width * m_display_width / 2;
	m_screen_matrix.uy = -m_view_height * m_display_height / 2;
	m_screen_matrix.az = m_max_z - m_min_z;
	m_screen_matrix.tx = m_view_x * m_display_width  + m_view_width * m_display_width / 2;
	m_screen_matrix.ty = m_view_y * m_display_height + m_view_height * m_display_height / 2;
	m_screen_matrix.tz = m_min_z;

	// Calculate near and far clip distances
	m_far_clip_distance  = m_far_clip_z;
	m_far_clip_distance2 = m_far_clip_distance * m_far_clip_distance;
	m_near_clip_distance = m_near_clip_z / cosf(m_horizontal_fov / 2);
	m_near_clip_distance2 = m_near_clip_distance * m_near_clip_distance;

	// Calculate clip planes
	float a,b,c,d,l;
#if defined(MATRIX_ROW_MAJOR)
	Matrix vp = m_view_matrix * m_projection_matrix;
#elif defined(MATRIX_COLUMN_MAJOR)
	Matrix vp = m_projection_matrix * m_view_matrix;
#endif
		// right clip plane
	a = vp.rw - vp.rx;
	b = vp.uw - vp.ux;
	c = vp.aw - vp.ax;
	d = vp.tw - vp.tx;
	l = sqrtf(a*a + b*b + c*c);
	m_right_clip_plane.a = a/l;
	m_right_clip_plane.b = b/l;
	m_right_clip_plane.c = c/l;
	m_right_clip_plane.d = d/l;

		// left clip plane
	a = vp.rw + vp.rx;
	b = vp.uw + vp.ux;
	c = vp.aw + vp.ax;
	d = vp.tw + vp.tx;
	l = sqrtf(a*a + b*b + c*c);
	m_left_clip_plane.a = a/l;
	m_left_clip_plane.b = b/l;
	m_left_clip_plane.c = c/l;
	m_left_clip_plane.d = d/l;

		// top clip plane
	a = vp.rw - vp.ry;
	b = vp.uw - vp.uy;
	c = vp.aw - vp.ay;
	d = vp.tw - vp.ty;
	l = sqrtf(a*a + b*b + c*c);
	m_top_clip_plane.a = a/l;
	m_top_clip_plane.b = b/l;
	m_top_clip_plane.c = c/l;
	m_top_clip_plane.d = d/l;

		// bottom clip plane
	a = vp.rw + vp.ry;
	b = vp.uw + vp.uy;
	c = vp.aw + vp.ay;
	d = vp.tw + vp.ty;
	l = sqrtf(a*a + b*b + c*c);
	m_bottom_clip_plane.a = a/l;
	m_bottom_clip_plane.b = b/l;
	m_bottom_clip_plane.c = c/l;
	m_bottom_clip_plane.d = d/l;

		// far clip plane
	a = vp.rw - vp.rz;
	b = vp.uw - vp.uz;
	c = vp.aw - vp.az;
	d = vp.tw - vp.tz;
	l = sqrtf(a*a + b*b + c*c);
	m_far_clip_plane.a = a/l;
	m_far_clip_plane.b = b/l;
	m_far_clip_plane.c = c/l;
	m_far_clip_plane.d = d/l;

		// near clip plane
	a = vp.rz;
	b = vp.uz;
	c = vp.az;
	d = vp.tz;
	l = sqrtf(a*a + b*b + c*c);
	m_near_clip_plane.a = a/l;
	m_near_clip_plane.b = b/l;
	m_near_clip_plane.c = c/l;
	m_near_clip_plane.d = d/l;

	// Combine the view, projection and screen matrices into a master transformation matrix
#if defined(MATRIX_ROW_MAJOR)
	m_transform_matrix = m_view_matrix * m_projection_matrix * m_screen_matrix;
#elif defined(MATRIX_COLUMN_MAJOR)
	m_transform_matrix = m_screen_matrix * m_projection_matrix * m_view_matrix;
#endif
}


float Viewport::GetDistanceFromNearClipPlane(Vector& position, float radius)
{
	// Calculate square of distance from object to camera
	float dx = position.x - m_view_position.x;
	float dy = position.y - m_view_position.y;
	float dz = position.z - m_view_position.z;
	float distance2 = (dx*dx) + (dy*dy) + (dz*dz);
	float radius2 = radius*radius;

	// Clip bounding sphere against far plane
	// Calculate square of distance as (f+r)^2 = f^2 + r^2 + 2fr
	//  where, f = far clip distance
	//         r = object radius
	float far_test = m_far_clip_distance2 + radius2 + (2 * m_far_clip_distance * radius);
	if (distance2 > far_test) return 0.0;

	// Clip bounding sphere against near plane
	// Calculate square of distance as (n+r)^2 = n^2 + r^2 + 2nr
	//  where, n = near clip distance
	//         r = object radius
	float near_test = m_near_clip_distance2 + radius2 + (2 * m_near_clip_distance * radius);
	if (distance2 < near_test) return 0.0;

	// Clip bounding sphere against left plane
	float dl = (m_left_clip_plane.a * position.x) + (m_left_clip_plane.b * position.y) + (m_left_clip_plane.c * position.z) + m_left_clip_plane.d;
	if (dl <= -radius) return 0.0;

	// Clip bounding sphere against right plane
	float dr = (m_right_clip_plane.a * position.x) + (m_right_clip_plane.b * position.y) + (m_right_clip_plane.c * position.z) + m_right_clip_plane.d;
	if (dr <= -radius) return 0.0;

	// Clip bounding sphere against bottom plane
	float db = (m_bottom_clip_plane.a * position.x) + (m_bottom_clip_plane.b * position.y) + (m_bottom_clip_plane.c * position.z) + m_bottom_clip_plane.d;
	if (db <= -radius) return 0.0;

	// Clip bounding sphere against top plane
	float dt = (m_top_clip_plane.a * position.x) + (m_top_clip_plane.b * position.y) + (m_top_clip_plane.c * position.z) + m_top_clip_plane.d;
	if (dt <= -radius) return 0.0;

	// Calculate and return object distance from near plane clip
	return sqrtf(distance2) - m_near_clip_distance - radius;
}
