#ifndef TME_ENGINE_NETWORK_ECS_UTILS_HPP
#define TME_ENGINE_NETWORK_ECS_UTILS_HPP

#include "TME/errorCode.hpp"
#include "TME/engine/networkEcs.hpp"

#define TME_ENTITY_ADD_COMPONENT(_ecs, _entityId, _component, _onError) \
	{ \
		tme::ErrorCode errorCode = _ecs->addComponentToEntity(_entityId, _component); \
		if (errorCode != tme::ErrorCode::Success) { \
			TME_ERROR_LOG("Failed to add component to entity %I32u. ErrorCode: %d", _entityId, static_cast<int>(errorCode)); \
			_onError \
		} \
	} \

#endif
