#include "projectile.hpp"

#include "gameMessage.hpp"

constexpr PROJECTILE_SPEED 500.0f;

uint32_t ProjectileManager::m_nextId = 0;
std::vector<Projectile> ProjectileManager::m_projectiles;

using namespace tme;

void ProjectileManager::AddProjectile(const Vector2f& _startPosition, const Vector2f& _direction, const engine::EntityId _shooterId)
{
	Projectile newProjectile;
	newProjectile.id = m_nextId++;
	newProjectile.position = _startPosition;
	newProjectile.direction = _direction;

	m_projectiles.push_back(newProjectile);

	std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
	for (size_t i = 0; i < playersId.size(); i++)
	{
		if (playersId[i] == _shooterId) continue;
		std::shared_ptr<engine::NewProjectileMessage> newProjectileMessage = std::make_shared<engine::NewProjectileMessage>();
	}
}

void ProjectileManager::UpdateProjectiles(const float _deltaTime)
{
	for (auto& projectile : m_projectiles)
	{
		projectile.position += projectile.direction * PROJECTILE_SPEED * _deltaTime;
	}
}
