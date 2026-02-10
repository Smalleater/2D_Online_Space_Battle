#ifndef TRA_ECS_COMPONENT_REGISTER_HPP
#define TRA_ECS_COMPONENT_REGISTER_HPP

#include "TRA/ecs/componentLibrary.hpp"

namespace tra::ecs
{
	template<typename T>
	struct ComponentRegister
	{
		ComponentRegister()
		{
			ComponentLibrary::registerComponent<T>();
		}
	};
}

#endif
