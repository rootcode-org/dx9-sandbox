/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <cstdlib>
#include "entity/entity_manager.h"
#include "entity/entity.h"

Entity::Entity(void)
{
	m_matrix.InitWithIdentity();
	m_radius = 0.0;
	EntityManager::GetInstance().Add(this);
}


Entity::~Entity(void)
{
	EntityManager::GetInstance().Remove(this);
}
