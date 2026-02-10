#ifndef TRA_ECS_WORLD_HPP
#define TRA_ECS_WORLD_HPP

#include "TRA/ecs/export.hpp"

#include <unordered_map>
#include <memory>
#include <functional>

#include "TRA/ecs/entityManager.hpp"
#include "TRA/ecs/archetype.hpp"
#include "TRA/ecs/componentLibrary.hpp"
#include "TRA/ecs/tag.hpp"
#include "TRA/ecs/ISystem.hpp"

namespace tra::ecs
{
	template<typename... T>
	struct WithComponent {};

	template<typename... T>
	struct WithoutComponent {};

	template<typename... T>
	struct WithTag {};

	template<typename... T>
	struct WithoutTag {};

	class World
	{
	public:
		TRA_ECS_API World();
		~World() = default;

		TRA_ECS_API Entity createEntity();
		TRA_ECS_API void destroyEntity(const Entity _entity);

		TRA_ECS_API void addBeginSystem(std::unique_ptr<ISystem> _ptr);
		TRA_ECS_API void addEndSystem(std::unique_ptr<ISystem> _ptr);
		TRA_ECS_API void updateBeginSystems();
		TRA_ECS_API void updateEndSystems();

		template<typename T>
		bool hasComponent(const Entity _entity)
		{
			const size_t& componentId = ComponentLibrary::getComponent<T>().m_id;
			return hasComponentImpl(_entity, componentId);
		}

		template<typename T>
		void addComponent(const Entity _entity, T&& _component)
		{
			const size_t componentId = ComponentLibrary::getComponent<T>().m_id;
			addComponentImpl(_entity, componentId,
				[&_component](uint8_t* _dst)
				{
					new(_dst) T(std::move(_component));
				}
			);
		}

		template<typename T>
		void removeComponent(const Entity _entity)
		{
			const size_t componentId = ComponentLibrary::getComponent<T>().m_id;
			removeComponentImpl(_entity, componentId);
		}

		template<typename T>
		T* getComponent(const Entity _entity)
		{
			const EntitySignature& signature = m_entityManager.getSignature(_entity);
			if (!signature.hasComponent(ComponentLibrary::getComponent<T>().m_id))
			{
				throw std::runtime_error("TRA ECS: Tried to access a component the entity does not have.");
			}

			EntityData& entityData = m_entityManager.getEntityData(_entity);
			return entityData.m_archetype->getComponentPtr<T>(entityData);
		}

		template<typename T>
		void setComponent(const Entity _entity, const T& _component)
		{
			const size_t componentId = ComponentLibrary::getComponent<T>().m_id;
			setComponentImpl(_entity, componentId,
				[&_component](uint8_t* _dst)
				{
					new(_dst) T(_component);
				}
			);
		}

		template<typename T>
		bool hasTag(const Entity _entity)
		{
			const size_t tagId = TagLibrary::getTagId<T>();
			return hasTagImpl(_entity, tagId);
		}

		template<typename T>
		void addTag(const Entity _entity)
		{
			const size_t tagId = TagLibrary::getTagId<T>();
			addTagImpl(_entity, tagId);
		}

		template<typename T>
		void removeTag(const Entity _entity)
		{
			const size_t tagId = TagLibrary::getTagId<T>();
			removeTagImpl(_entity, tagId);
		}

		template<typename... WithComps, typename... WithoutComps, 
			typename... WithTags, typename... WithoutTags>
		std::vector<std::tuple<Entity, WithComps*...>> queryEntities(
			WithComponent<WithComps...> _withComponents, WithoutComponent<WithoutComps...> _withoutComponents = WithoutComponent<>{},
			WithTag<WithTags...> _withTags = WithTag<>{}, WithoutTag<WithoutTags...> _withoutTags= WithoutTag<>{})
		{
			SignatureKey cashKey;

			(cashKey.addKey(ComponentLibrary::getComponent<WithComps>().m_id), ...);

			auto it = m_queryArchetypeCache.find(cashKey);
			if (it == m_queryArchetypeCache.end())
			{
				SignatureKey withComponent;
				SignatureKey withoutComponent;

				(withComponent.addKey(ComponentLibrary::getComponent<WithComps>().m_id), ...);
				(withoutComponent.addKey(ComponentLibrary::getComponent<WithoutComps>().m_id), ...);

				std::vector<Archetype*> archetypes;
				for (size_t i = 0; i < m_archetypes.size(); ++i)
				{
					Archetype* archetype = m_archetypes.at(i).get();
					if (SignatureKey::matches(archetype->getSignatureKey(), withComponent, withoutComponent))
					{
						archetypes.push_back(archetype);
					}
				}

				auto result = m_queryArchetypeCache.insert({ cashKey, std::move(archetypes) });
				it = result.first;
			}

			SignatureKey withTag;
			SignatureKey withoutTag;

			(withTag.addKey(TagLibrary::getTagId<WithTags>()), ...);
			(withoutTag.addKey(TagLibrary::getTagId<WithoutTags>()), ...);

			std::vector<std::tuple<Entity, WithComps*...>> result;

			for (auto archetype : it->second)
			{
				for (EntityId entityId : archetype->getEntitiesId())
				{
					Entity entity = m_entityManager.getEntityById(entityId);
					EntitySignature signature = m_entityManager.getSignature(entity);

					if (SignatureKey::matches(signature.m_tags, withTag, withoutTag))
					{
						EntityData& entityData = m_entityManager.getEntityData(entity);

						result.emplace_back(entity, archetype->getComponentPtr<WithComps>(entityData)...);
					}
				}
			}

			return result;
		}

	private:
		EntityManager m_entityManager;
		std::vector<std::unique_ptr<Archetype>> m_archetypes;
		std::unordered_map<SignatureKey, size_t> m_archetypeLookUp;
		std::unordered_map<SignatureKey, std::vector<Archetype*>> m_queryArchetypeCache;
		std::vector<std::unique_ptr<ISystem>> m_beginSystems;
		std::vector<std::unique_ptr<ISystem>> m_endSystems;

		TRA_ECS_API bool hasComponentImpl(const Entity _entity, const size_t _componentId);
		TRA_ECS_API void addComponentImpl(const Entity _entity, const size_t _componentId, std::function<void(uint8_t*)> _constructor);
		TRA_ECS_API void removeComponentImpl(const Entity _entity, const size_t _componentId);
		TRA_ECS_API void setComponentImpl(const Entity _entity, const size_t _componentId, std::function<void(uint8_t*)> _constructor);

		TRA_ECS_API bool hasTagImpl(const Entity _entity, const size_t _tagId);
		TRA_ECS_API void addTagImpl(const Entity _entity, const size_t _tagId);
		TRA_ECS_API void removeTagImpl(const Entity _entity, const size_t _tagId);

		Archetype* getOrCreateArchetype(const SignatureKey _key);
		void copyComponentsToArchetype(Archetype* _srcArch, Archetype* _dstArch,
			const EntityData& _srcData, const EntityData& _dstData, const EntitySignature& _entitySignature);
		void removeEntityFromArchetype(Archetype* _archetype, EntityData& _entityData);
	};
}

#endif
