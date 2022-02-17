/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include "core/list.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/plane.h"

class Viewport {
public:
	Viewport (void);

	void  SetViewPlacement(const Matrix& m);
	void  SetViewFrustrum(float horizontal_fov, float near_clip_z, float far_clip_z);
	void  SetViewDimensions(float x, float y, float width, float height, float min_z, float max_z);
	void  Recalculate(int display_width, int display_height);
	float GetDistanceFromNearClipPlane(Vector& position, float radius);

	const Matrix& GetViewMatrix       (void) const { return m_view_matrix; }
	const Matrix& GetProjectionMatrix (void) const { return m_projection_matrix; }
	const Matrix& GetScreenMatrix     (void) const { return m_screen_matrix; }
	const Matrix& GetTransformMatrix  (void) const { return m_transform_matrix; }
	const Vector& GetViewPosition     (void) const { return m_view_position; }
	const Plane&  GetRightClipPlane   (void) const { return m_right_clip_plane; }
	const Plane&  GetLeftClipPlane    (void) const { return m_left_clip_plane; }
	const Plane&  GetTopClipPlane     (void) const { return m_top_clip_plane; }
	const Plane&  GetBottomClipPlane  (void) const { return m_bottom_clip_plane; }
	const Plane&  GetFarClipPlane     (void) const { return m_near_clip_plane; }
	const Plane&  GetNearClipPlane    (void) const { return m_far_clip_plane; }
	unsigned int  GetViewportX        (void) const { return (unsigned int)(m_view_x * m_display_width); }
	unsigned int  GetViewportY        (void) const { return (unsigned int)(m_view_y * m_display_height); }
	unsigned int  GetViewportWidth    (void) const { return (unsigned int)(m_view_width * m_display_width); }
	unsigned int  GetViewportHeight   (void) const { return (unsigned int)(m_view_height * m_display_height); }
	float         GetNearClipZ        (void) const { return m_near_clip_z; }
	float         GetFarClipZ         (void) const { return m_far_clip_z; }
	float         GetFOV              (void) const { return m_horizontal_fov; }
	float         GetMinZ             (void) const { return m_min_z; }
	float         GetMaxZ             (void) const { return m_min_z; }

private:
	Matrix m_view_matrix;
	float  m_horizontal_fov;		// in degrees
	float  m_near_clip_z;
	float  m_far_clip_z;
	float  m_display_width;			// in pixels
	float  m_display_height;		// "
	float  m_view_x;				// 0.0 to 1.0
	float  m_view_y;				// "
	float  m_view_width;			// "
	float  m_view_height;			// "
	float  m_min_z;
	float  m_max_z;

	// Calculated values
	float  m_far_clip_distance;
	float  m_far_clip_distance2;
	float  m_near_clip_distance;
	float  m_near_clip_distance2;
	Matrix m_projection_matrix;
	Matrix m_screen_matrix;
	Matrix m_transform_matrix;		// Combination of view, projection and screen matrices
	Vector m_view_position;
	Plane  m_left_clip_plane;
	Plane  m_right_clip_plane;
	Plane  m_top_clip_plane;
	Plane  m_bottom_clip_plane;
	Plane  m_far_clip_plane;
	Plane  m_near_clip_plane;
};
