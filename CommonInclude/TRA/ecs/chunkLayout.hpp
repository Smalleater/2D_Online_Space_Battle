#ifndef TRA_ECS_CHUNK_LAYOUT_HPP
#define TRA_ECS_CHUNK_LAYOUT_HPP

#include <cstdint>
#include <vector>

#include "TRA/ecs/chunkColumn.hpp"

namespace tra::ecs
{
	struct ChunkLayout
	{
		size_t m_chunkSize;
		size_t m_capacity;

		std::vector<ChunkColumn> m_columns;
	};
}

#endif
