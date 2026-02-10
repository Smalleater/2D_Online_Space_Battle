#ifndef TRA_ECS_ENTITY_HPP
#define TRA_ECS_ENTITY_HPP

#include <cstdint>

namespace tra::ecs
{
	using EntityId = uint32_t;
	using EntityVersion = uint32_t;

	constexpr uint8_t ENTITY_INDEX_BITS = 24;
	constexpr uint8_t ENTITY_VERSION_BITS = 8;

	constexpr uint32_t ENTITY_INDEX_MASK = (1u << ENTITY_INDEX_BITS) - 1u;
	constexpr uint32_t ENTITY_VERSION_MASK = (1u << ENTITY_VERSION_BITS) - 1u;

	constexpr uint32_t MAX_ENTITIES = (1u << ENTITY_INDEX_BITS) - 1u;

	struct Entity
	{
		uint32_t m_idAndVersion = 0;

		bool operator==(Entity& _other) const
		{
			return m_idAndVersion == _other.m_idAndVersion;
		}

		bool operator!=(Entity& _other) const
		{
			return !(*this == _other);
		}

		constexpr EntityId id() const
		{
			return m_idAndVersion & ENTITY_INDEX_MASK;
		}

		constexpr EntityVersion version() const
		{
			return (m_idAndVersion >> ENTITY_INDEX_BITS) & ENTITY_VERSION_MASK;
		}

		constexpr bool isNull() const
		{
			return m_idAndVersion == 0;
		}
	};

	constexpr Entity NULL_ENTITY = Entity{};

	constexpr Entity makeEntity(EntityId _id, EntityVersion _version)
	{
		return Entity{ (_version << ENTITY_INDEX_BITS) | (_id & ENTITY_INDEX_MASK) };
	}
}

#endif
