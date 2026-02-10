#ifndef TRA_ECS_ENTITY_SIGNATURE_HPP
#define TRA_ECS_ENTITY_SIGNATURE_HPP

#include "TRA/ecs/export.hpp"

#include <array>
#include <cstdint>

#include "TRA/ecs/signatureKey.hpp"

namespace tra::ecs
{
	constexpr size_t MAX_COMPONENTS = 64;
	constexpr size_t MAX_TAGS = 64;

	constexpr size_t COMPONENT_BLOCK = (MAX_COMPONENTS + 63) / 64;
	constexpr size_t TAG_BLOCK = (MAX_TAGS + 63) / 64;

	struct EntitySignature
	{
		SignatureKey m_components;
		SignatureKey m_tags;

		bool operator==(const EntitySignature& _other) const;
		bool operator!=(const EntitySignature& _other) const;

		TRA_ECS_API void addComponent(const size_t _componentId);
		TRA_ECS_API void removeComponent(const size_t _componentId);

		TRA_ECS_API bool hasComponent(const size_t _componentId) const;

		TRA_ECS_API void addTag(const size_t _tagId);
		TRA_ECS_API void removeTag(const size_t _tagId);

		TRA_ECS_API bool hasTag(const size_t _tagId) const;
	};

	constexpr EntitySignature NULL_ENTITY_SIGNATURE = EntitySignature{};
}

#endif
