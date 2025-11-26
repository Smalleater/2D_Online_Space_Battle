#ifndef TME_ENGINE_NETWORK_ECS_HPP
#define TME_ENGINE_NETWORK_ECS_HPP

#include "TME/export.hpp"

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <utility>

#include "TME/errorCode.hpp"
#include "TME/debugUtils.hpp"

namespace tme::engine
{
	using EntityId = uint32_t;

	struct INetworkComponent;
	struct INetworkSystem;

	template<typename ComponentType>
	struct SparseSet
	{
		std::vector<std::shared_ptr<ComponentType>> m_dense;
		std::unordered_map<EntityId, size_t> m_sparse;

		void insert(EntityId _entityId, std::shared_ptr<ComponentType> _component)
		{
			m_sparse[_entityId] = m_dense.size();
			m_dense.push_back(_component);
		}

		void remove(EntityId _entityId)
		{
			auto it = m_sparse.find(_entityId);
			if (it != m_sparse.end())
			{
				size_t index = it->second;
				size_t last = m_dense.size() - 1;
				if (index != last) {
					m_dense[index] = m_dense[last];
					for (auto& pair : m_sparse) {
						if (pair.second == last) {
							pair.second = index;
							break;
						}
					}
				}

				m_dense.pop_back();
				m_sparse.erase(it);
			}
		}

		std::pair<ErrorCode, std::weak_ptr<ComponentType>> get(EntityId _entityId)
		{
			auto it = m_sparse.find(_entityId);
			if (it != m_sparse.end())
			{
				return { ErrorCode::Success, m_dense[it->second] };
			}

			return { ErrorCode::EntityDoesNotHaveComponent , std::weak_ptr<ComponentType>() };
		}

		bool hasComponent(EntityId _entityId)
		{
			return m_sparse.find(_entityId) != m_sparse.end();
		}
	};

	class NetworkEcs
	{
	public:
		NetworkEcs() = default;
		~NetworkEcs() = default;

		EntityId createEntity();
		bool isEntityValid(EntityId _entityId);
		void destroyEntity(EntityId _entityId);

		void registerBeginUpdateSystem(std::shared_ptr<INetworkSystem> _system);
		void registerEndUpdateSystem(std::shared_ptr<INetworkSystem> _system);

		void beginUpdate();
		void endUpdate();

		template<typename ComponentType>
		ErrorCode addComponentToEntity(EntityId _entityId, std::shared_ptr<ComponentType> _component)
		{
			auto store = getOrCreateComponentStore<ComponentType>().lock();
			if (!store)
			{
				return ErrorCode::ComponentStoreUnavailable;
			}

			if (store->hasComponent(_entityId))
			{
				return ErrorCode::EntityAlreadyHasComponent;
			}

			store->insert(_entityId, _component);

			return ErrorCode::Success;
		}

		template<typename ComponentType>
		bool hasComponent(EntityId _entityId)
		{
			auto store = getOrCreateComponentStore<ComponentType>().lock();
			if (!store)
			{
				return false;
			}

			return store->hasComponent(_entityId);
		}

		template<typename ComponentType>
		std::pair<ErrorCode, std::weak_ptr<ComponentType>> getComponentOfEntity(EntityId _entityId)
		{
			auto store = getOrCreateComponentStore<ComponentType>().lock();
			if (!store)
			{
				return { ErrorCode::ComponentStoreUnavailable , std::weak_ptr<ComponentType>() };
			}

			return store->get(_entityId);
		}

		template<typename ...ComponentType>
		std::vector<EntityId> queryIds()
		{
			std::vector<EntityId> result;
			for (const auto& entityId : m_entities)
			{
				bool hasAllComponents = true;
				((hasAllComponents = (hasAllComponents && hasComponent<ComponentType>(entityId))), ...);
				if (hasAllComponents)
				{
					result.push_back(entityId);
				}
			}

			return result;
		}

		template<typename ...ComponentType>
		std::vector<std::tuple<EntityId, std::shared_ptr<ComponentType>...>> query()
		{
			std::vector<std::tuple<EntityId, std::shared_ptr<ComponentType>...>> result;
			for (const auto& entityId : m_entities)
			{
				bool hasAllComponents = true;
				((hasAllComponents = (hasAllComponents && hasComponent<ComponentType>(entityId))), ...);
				if (hasAllComponents)
				{
					result.emplace_back(entityId, getComponentOfEntity<ComponentType>(entityId).second.lock()...);
				}
			}

			return result;
		}

		template<typename ComponentType>
		ErrorCode removeComponentFromEntity(EntityId _entityId)
		{
			auto store = getOrCreateComponentStore<ComponentType>().lock();
			if (!store)
			{
				return ErrorCode::ComponentStoreUnavailable;
			}

			store->remove(_entityId);

			return ErrorCode::Success;
		}

	private:
		EntityId m_nextEntityId = 1;
		std::unordered_set<EntityId> m_entities;

		std::unordered_map<size_t, std::shared_ptr<void>> m_componentStores;

		std::unordered_set<std::shared_ptr<INetworkSystem>> m_registerBeginUpdateSystem;
		std::unordered_set<std::shared_ptr<INetworkSystem>> m_registerEndUpdateSystem;
		std::vector<std::shared_ptr<INetworkSystem>> m_beginUpdateSystems;
		std::vector<std::shared_ptr<INetworkSystem>> m_endUpdateSystems;

		template<typename ComponentType>
		std::weak_ptr<SparseSet<ComponentType>> getOrCreateComponentStore()
		{
			static_assert(std::is_base_of<INetworkComponent, ComponentType>::value, "ComponentType must derive from INetworkComponent");

			size_t hashCode = typeid(ComponentType).hash_code();
			auto it = m_componentStores.find(hashCode);
			if (it == m_componentStores.end())
			{
				std::shared_ptr<SparseSet<ComponentType>> newStore = std::make_shared<SparseSet<ComponentType>>();
				m_componentStores[hashCode] = newStore;
				return std::weak_ptr<SparseSet<ComponentType>>(newStore);
			}

			return std::static_pointer_cast<SparseSet<ComponentType>>(it->second);
		}
	};
}

#endif
