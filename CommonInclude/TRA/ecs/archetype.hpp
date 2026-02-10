#ifndef TRA_ECS_ARCHETYPE_HPP
#define TRA_ECS_ARCHETYPE_HPP

#include "TRA/ecs/export.hpp"

#include <optional>
#include <utility>

#include "TRA/ecs/chunk.hpp"
#include "TRA/ecs/entity.hpp"
#include "TRA/ecs/entityData.hpp"
#include "TRA/ecs/entitySignature.hpp"
#include "TRA/ecs/signatureKey.hpp"
#include "TRA/ecs/componentLibrary.hpp"

namespace tra::ecs
{
	constexpr int MAX_CHUNK_SIZE = 64 * 1024;

	class Archetype
	{
	public:
		TRA_ECS_API Archetype(const SignatureKey& _signature);
		~Archetype() = default;

		TRA_ECS_API SignatureKey getSignatureKey();

		TRA_ECS_API void addEntity(const Entity _entity, EntityData& _entityData);
		TRA_ECS_API std::optional<std::pair<EntityId, size_t>> removeEntity(EntityData& _entityData);
		
		TRA_ECS_API std::vector<EntityId> getEntitiesId();

		template<typename T>
		T* getComponentPtr(const EntityData& _entityData)
		{
			size_t componentId = ComponentLibrary::getComponent<T>().m_id;
			return reinterpret_cast<T*>(getComponentPtr(_entityData, componentId));
		}

		TRA_ECS_API uint8_t* getComponentPtr(const EntityData& _entityData, const size_t _componentid);

	private:
		const SignatureKey m_signatureKey;

		std::vector<size_t> m_componentIds;
		ChunkLayout m_layout;

		std::vector<Chunk> m_chunks;
		std::vector<size_t> m_freeChunkIndices;

		std::vector<EntityId> m_entitiesIdCache;

		constexpr size_t alignUp(size_t _value, size_t _alignment)
		{
			return (_value + _alignment - 1) & ~(_alignment - 1);
		}

		ChunkLayout buildChunkLayout();
		Chunk createChunk();
	};
}

#endif
