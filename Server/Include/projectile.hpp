#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <cstdint>
#include <vector>

#include "TME/server/server.hpp"

#include "vector.hpp"

struct Projectile
{
	uint32_t id;
	Vector2f position;
	Vector2f direction;
};

class ProjectileManager
{
public:
	static void AddProjectile(const Vector2f& _startPosition, const Vector2f& _direction, const tme::engine::EntityId _shooterId);
	static void UpdateProjectiles(const float _deltaTime);

private:
	static uint32_t m_nextId;
	static std::vector<Projectile> m_projectiles;
};

#endif
