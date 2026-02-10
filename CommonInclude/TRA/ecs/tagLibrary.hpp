#ifndef TRA_ECS_TAG_LIBRARY_HPP
#define TRA_ECS_TAG_LIBRARY_HPP

#include "TRA/ecs/export.hpp"

#include <unordered_map>
#include <typeindex>
#include <stdexcept>

#include "TRA/ecs/entitySignature.hpp"

namespace tra::ecs
{
	class TagLibrary
	{
	public:
		template<typename T>
		static void registerTag()
		{
			size_t id = m_tagtLookUp.size();

			if (id >= MAX_TAGS)
			{
				throw std::runtime_error("TRA ECS: Exceeding tag limit.");
			}

			m_tagtLookUp.insert({ typeid(T), id });
		}

		template<typename T>
		static size_t getTagId()
		{
			return m_tagtLookUp.at(typeid(T));
		}

	private:
		TRA_ECS_API static std::unordered_map<std::type_index, size_t> m_tagtLookUp;
	};
}

#endif
