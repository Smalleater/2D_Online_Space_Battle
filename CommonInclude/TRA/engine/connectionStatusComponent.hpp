#ifndef TRA_ENGINE_CONNECTION_STATUS_COMPONENT_HPP
#define TRA_ENGINE_CONNECTION_STATUS_COMPONENT_HPP

#include "TRA/engine/iNetworkComponent.hpp"

namespace tra::engine
{
	struct ConnectedComponentTag : INetworkComponent
	{

	};

	struct ListeningComponentTag : INetworkComponent
	{

	};
}

#endif
