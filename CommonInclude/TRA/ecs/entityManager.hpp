#ifndef TRA_ECS_ENTITY_MANAGER_HPP
#define TRA_ECS_ENTITY_MANAGER_HPP

#include "TRA/ecs/export.hpp"

#include <vector>

#include "TRA/ecs/entity.hpp"
#include "TRA/ecs/entityData.hpp"
#include "TRA/ecs/entitySignature.hpp"

namespace tra::ecs
{
	class EntityManager
	{
	public:
		TRA_ECS_API EntityManager();
		~EntityManager() = default;

		Entity createEntity();
		void deleteEntity(Entity _entity);

		TRA_ECS_API Entity& getEntityById(const EntityId _id);
		TRA_ECS_API EntityData& getEntityData(const Entity _entity);
		TRA_ECS_API EntitySignature& getSignature(const Entity _entity);

	private:
		EntityId m_nextFreeId;

		std::vector<EntityId> m_freeIds;

		std::vector<Entity> m_entities;
		std::vector<EntityData> m_entitiesData;
		std::vector<EntitySignature> m_signatures;
	};
}

#endif
