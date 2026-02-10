#ifndef TRA_ECS_ARCHETYPE_KEY_HPP
#define TRA_ECS_ARCHETYPE_KEY_HPP

#include "TRA/ecs/export.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace tra::ecs
{
	constexpr size_t MAX_ELEMENTS = 64;
	constexpr size_t BLOCK = (MAX_ELEMENTS + 63) / 64;

	struct SignatureKey
	{
		std::array<uint64_t, BLOCK> m_keys{};

		SignatureKey() = default;
		SignatureKey(const SignatureKey&) = default;
		~SignatureKey() = default;

		TRA_ECS_API bool operator==(const SignatureKey& _other) const;
		bool operator!=(const SignatureKey& _other) const;

		TRA_ECS_API static bool matches(const SignatureKey& _signatureKey, const SignatureKey& _required, const SignatureKey& _excluded);

		TRA_ECS_API void addKey(const size_t _key);
		TRA_ECS_API void removeKey(const size_t _key);

		bool hasKey(const size_t _key) const;
	};
}

namespace std
{
	template<>
	struct hash<tra::ecs::SignatureKey>
	{
		size_t operator()(const tra::ecs::SignatureKey& _signaturekey) const
		{
			size_t hash = 0;

			for (size_t i = 0; i < tra::ecs::BLOCK; i++)
			{
				hash ^= std::hash<uint64_t>{}(_signaturekey.m_keys[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			}

			return hash;
		}
	};
}

#endif
