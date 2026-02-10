#ifndef TRA_ECS_TAG_HPP
#define TRA_ECS_TAG_HPP

#include "TRA/ecs/tagRegister.hpp"

#define TRA_ECS_REGISTER_TAG(NAME) \
struct NAME \
{ \
private: \
	static inline tra::ecs::TagRegister<NAME> m_registrar{}; \
};\

#endif
