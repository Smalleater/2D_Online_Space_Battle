#ifndef TRA_ECS_CHUNK_HPP
#define TRA_ECS_CHUNK_HPP

#include "TRA/ecs/chunkLayout.hpp"

namespace tra::ecs
{
	struct Chunk
	{
		uint8_t* m_data;
		size_t m_count;
		size_t m_capacity;

		const ChunkLayout* m_layout;
	};
}

#endif
