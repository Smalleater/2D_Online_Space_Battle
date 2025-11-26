#ifndef TME_ENGINE_I_SERIALIZABLE_COMPONENT_HPP
#define TME_ENGINE_I_SERIALIZABLE_COMPONENT_HPP

#include "TME/engine/INetworkComponent.hpp"

#include <vector>
#include <cstdint>

namespace tme::engine
{
	struct ISerializableComponent : public INetworkComponent
	{
		virtual std::vector<uint8_t> serialize() const = 0;
		virtual void deserialize(std::vector<uint8_t> _buffer) = 0;
	};
}

#endif
