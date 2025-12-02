#ifndef TRA_ENGINE_NETWORK_ECS_UTILS_HPP
#define TRA_ENGINE_NETWORK_ECS_UTILS_HPP

#include "TRA/errorCode.hpp"
#include "TRA/engine/networkEcs.hpp"

#define TRA_ENTITY_ADD_COMPONENT(_ecs, _entityId, _component, _onError) \
	{ \
		tra::ErrorCode errorCode = _ecs->addComponentToEntity(_entityId, _component); \
		if (errorCode != tra::ErrorCode::Success) { \
			TRA_ERROR_LOG("Failed to add component to entity %I32u. ErrorCode: %d", _entityId, static_cast<int>(errorCode)); \
			_onError \
		} \
	} \

#endif
