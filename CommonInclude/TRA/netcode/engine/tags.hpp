#ifndef TRA_NETCODE_ENGINE_TAGS_HPP
#define TRA_NETCODE_ENGINE_TAGS_HPP

#include "TRA/ecs/tag.hpp"

namespace tra::netcode::engine::tags
{
	TRA_ECS_REGISTER_TAG(ListeningTag)
	TRA_ECS_REGISTER_TAG(NewConnectionTag)
	TRA_ECS_REGISTER_TAG(ConnectedTag)
	TRA_ECS_REGISTER_TAG(DisconnectedTag)
	TRA_ECS_REGISTER_TAG(SelfTag)
}

#endif
