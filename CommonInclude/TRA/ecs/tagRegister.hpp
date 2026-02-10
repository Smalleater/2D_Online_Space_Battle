#ifndef TRA_ECS_TAG_REGISTER_HPP
#define TRA_ECS_TAG_REGISTER_HPP

#include "TRA/ecs/tagLibrary.hpp"

namespace tra::ecs
{
	template<typename T>
	struct TagRegister
	{
		TagRegister()
		{
			TagLibrary::registerTag<T>();
		}
	};
}

#endif
