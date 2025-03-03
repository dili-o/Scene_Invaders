#pragma once

#include "Entity.h"
#include <vector>
#include <map>

typedef std::vector<std::shared_ptr<Entity>> EntityVec; // Contains all entities of a certain tag
typedef std::map<std::string, EntityVec>	 EntityMap;

class EntityManager
{
private:
	EntityVec	m_entities;
	EntityVec	m_entitiesToAdd; // Delayed adding of entities
	EntityMap	m_entityMap;
	size_t		m_totalEntities = 0;

	void removeDeadEntities(EntityVec& vec);

public:

	EntityManager();

	void update();

	std::shared_ptr<Entity> addEntity(const std::string& tag);

	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);

};

