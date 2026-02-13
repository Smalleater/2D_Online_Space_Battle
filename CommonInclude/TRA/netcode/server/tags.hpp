#ifndef TRA_NETCODE_SERVER_TAGS_HPP
#define TRA_NETCODE_SERVER_TAGS_HPP

#include "TRA/ecs/tag.hpp"

namespace tra::netcode::server::tags
{
	TRA_ECS_REGISTER_TAG(ClientIsReadyTag);
	TRA_ECS_REGISTER_TAG(WaitingClientIsReadyTag);
}

#endif
