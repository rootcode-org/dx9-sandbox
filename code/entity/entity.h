/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include "core/list.h"
#include "math/matrix.h"
#include "graphics/d3d9.h"

class Viewport;

// Base class for all entity classes
class Entity {
	friend class Scene;
	friend class EntityManager;

public:
	Entity(void);
	~Entity(void);

protected:
	Matrix m_matrix;		// position and orientation
	float  m_radius;		// radius of object

private:
	ListNode<Entity> m_entity_node;		// Node for entity manager list
	ListNode<Entity> m_scene_node;		// Node for scene manager list

	virtual void Update(float frame_time) { frame_time; };
	virtual void Draw(Viewport& viewport) { viewport; };
	virtual void CreateResources(void) {};
	virtual void DestroyResources(void) {};
};
