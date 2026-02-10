#ifndef TRA_ECS_CHUNK_COLUMNS_HPP
#define TRA_ECS_CHUNK_COLUMNS_HPP

#include <cstdint>

namespace tra::ecs
{
	struct ChunkColumn
	{
		size_t m_componentId;
		size_t m_offset;
		size_t m_stride;
	};
}

#endif
