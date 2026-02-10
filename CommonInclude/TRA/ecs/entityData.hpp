#ifndef TRA_ECS_ENTITY_DATA_HPP
#define TRA_ECS_ENTITY_DATA_HPP

#include <cstdint>

namespace tra::ecs
{
	class Archetype;

	struct EntityData
	{
		Archetype* m_archetype = nullptr;
		size_t m_chunkIndex = SIZE_MAX;
		size_t m_row = SIZE_MAX;
	};
}

#endif
