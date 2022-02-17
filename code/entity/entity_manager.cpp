/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "entity/entity_manager.h"

static EntityManager entity_manager;

EntityManager& EntityManager::GetInstance(void)
{
	return entity_manager;
}


EntityManager::~EntityManager(void)
{
	// TODO: display leak warning if list is not empty
}


void EntityManager::Add(Entity* entity)
{
	m_entities.InsertTail(entity);
}


void EntityManager::Remove(Entity* entity)
{
	m_entities.Remove(entity);
}


void EntityManager::UpdateAll(float frame_time)
{
	for (Entity* entity = m_entities.IterateFirst(); entity; entity = m_entities.IterateNext())
	{
		entity->Update(frame_time);
	}
}


void EntityManager::DrawAll(Viewport& viewport)
{
	for (Entity* entity = m_entities.IterateFirst(); entity; entity = m_entities.IterateNext())
	{
		entity->Draw(viewport);
	}
}


void EntityManager::CreateAllResources(void)
{
	for (Entity* entity = m_entities.IterateFirst(); entity; entity = m_entities.IterateNext())
	{
		entity->CreateResources();
	}
}


void EntityManager::DestroyAllResources(void)
{
	for (Entity* entity = m_entities.IterateFirst(); entity; entity = m_entities.IterateNext())
	{
		entity->DestroyResources();
	}
}
