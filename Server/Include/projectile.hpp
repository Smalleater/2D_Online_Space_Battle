#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <cstdint>
#include <vector>

#include "vector.hpp"

struct Projectile
{
	uint32_t m_id;
	Vector2f m_position;
	Vector2f m_direction;
};

class ProjectileManager
{
public:
	static void createProjectile(const Vector2f& position, const Vector2f& direction);
	static void updateProjectiles(const float deltaTime);

private:
	static std::vector<Projectile> m_projectiles;
	static uint32_t m_nextProjectileId;
};

#endif // !PROJECTILE_HPP
