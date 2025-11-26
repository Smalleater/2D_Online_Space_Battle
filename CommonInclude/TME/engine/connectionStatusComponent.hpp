#ifndef TME_ENGINE_CONNECTION_STATUS_COMPONENT_HPP
#define TME_ENGINE_CONNECTION_STATUS_COMPONENT_HPP

#include "TME/engine/iNetworkComponent.hpp"

namespace tme::engine
{
	struct ConnectedComponentTag : INetworkComponent
	{

	};

	struct ListeningComponentTag : INetworkComponent
	{

	};
}

#endif
