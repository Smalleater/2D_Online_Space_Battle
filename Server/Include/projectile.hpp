#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <cstdint>
#include <vector>

#include <TRA/server/server.hpp>
#include "vector.hpp"

struct Projectile
{
	uint32_t m_id;
	tra::engine::EntityId m_shooterId;
	Vector2f m_position;
	Vector2f m_direction;
	float lifetime;
};

class ProjectileManager
{
public:
	static void createProjectile(const tra::engine::EntityId _m_shooterId, const Vector2f& position, const Vector2f& direction);
	static void updateProjectiles(const float deltaTime);

private:
	static std::vector<Projectile> m_projectiles;
	static uint32_t m_nextProjectileId;

	static void removeProjectile(size_t index);
	static bool checkCollision(const Projectile& _projectil);
};

#endif
