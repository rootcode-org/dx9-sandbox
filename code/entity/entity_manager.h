/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

#include <cstdlib>
#include "entity/entity.h"

class Viewport;

class EntityManager {
public:
	EntityManager(void) {};
	~EntityManager(void);

	void Add(Entity* entity);
	void Remove(Entity* entity);
	void UpdateAll(float frame_time);
	void DrawAll(Viewport& viewport);
	void CreateAllResources(void);
	void DestroyAllResources(void);

	static EntityManager& GetInstance(void);

private:
	List<Entity, offsetof(Entity, m_entity_node)> m_entities;
};
