#ifndef TRA_ECS_COMPONENT_LIBRARY_HPP
#define TRA_ECS_COMPONENT_LIBRARY_HPP

#include "TRA/ecs/export.hpp"

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

#include "TRA/ecs/entitySignature.hpp"
#include "TRA/ecs/componentInfo.hpp"

namespace tra::ecs
{
	class ComponentLibrary
	{
	public:
		template<typename T>
		static void registerComponent()
		{
			size_t id = m_components.size();

			if (id >= MAX_COMPONENTS)
			{
				throw std::runtime_error("TRA ECS: Exceeding component limit.");
			}

			ComponentInfo info;
			info.m_size = sizeof(T);
			info.m_alignment = alignof(T);
			info.m_id = id;
			info.m_name = typeid(T).name();


			info.m_moveFunc = [](void* _dst, void* _src)
				{
					new(_dst) T(std::move(*static_cast<T*>(_src)));
					static_cast<T*>(_src)->~T();
				};

			if constexpr (!std::is_destructible_v<T>)
			{
				info.m_destroyFunc = [](void* _ptr) { static_cast<T*>(_ptr)->~T(); };
			}
			else
			{
				info.m_destroyFunc = nullptr;
			}

			m_components.push_back(info);
			m_componentLookUp.insert({ typeid(T), id });
		}

		template<typename T>
		static const ComponentInfo& getComponent()
		{
			return m_components.at(m_componentLookUp.at(typeid(T)));
		}

		TRA_ECS_API static const ComponentInfo& getComponent(size_t _id);
		TRA_ECS_API static const size_t getCount();

	private:
		TRA_ECS_API static std::vector<ComponentInfo> m_components;
		TRA_ECS_API static std::unordered_map<std::type_index, size_t> m_componentLookUp;
	};
}

#endif
