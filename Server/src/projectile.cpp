#include "projectile.hpp"

#include <TRA/netcode/server/server.hpp>
#include <TRA/netcode/server/tags.hpp>

#include <TRA/netcode/engine/tags.hpp>

#include "gameMessage.hpp"
#include "tags.hpp"
#include "player.hpp"

constexpr float WORLD_SIZE = 800.0f;
constexpr float PROJECTILE_LIFETIME = 1.0f;
constexpr float PROJECTILE_RADIUS = 5.0f;
constexpr float PROJECTILE_SPEED = 500.0f;

std::vector<Projectile> ProjectileManager::m_projectiles;
uint32_t ProjectileManager::m_nextProjectileId = 0;

using namespace tra;
using namespace tra::netcode;

void ProjectileManager::createProjectile(const tra::ecs::Entity _m_shooter, const Vector2f& position, const Vector2f& direction)
{
	Projectile projectile;
	projectile.m_id = m_nextProjectileId++;
	projectile.m_shooter = _m_shooter;
	projectile.m_position = position;
	projectile.m_direction = direction;
	projectile.lifetime = 0.0f;
	m_projectiles.push_back(projectile);

	auto newProjectileMessage = std::make_shared<message::NewProjectileMessage>();
	newProjectileMessage->m_projectileId = projectile.m_id;
	newProjectileMessage->m_positionX = projectile.m_position.x;
	newProjectileMessage->m_positionY = projectile.m_position.y;
	newProjectileMessage->m_directionX = projectile.m_direction.x;
	newProjectileMessage->m_directionY = projectile.m_direction.y;

	for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<engine::tags::ConnectedTag, server::tags::ClientIsReadyTag, PlayerIsInitialized>{}))
	{
		server::Server::Get()->sendTcpMessage(entity, newProjectileMessage);
	}
}

void ProjectileManager::updateProjectiles(const float deltaTime)
{
	auto queryResult = server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<tags::ConnectedTag, server::tags::ClientIsReadyTag, PlayerIsInitialized>{});

	for (size_t i = 0; i < m_projectiles.size(); i++)
	{
		m_projectiles[i].lifetime += deltaTime;
		if (m_projectiles[i].lifetime >= PROJECTILE_LIFETIME)
		{
			removeProjectile(i);
			i--;
			continue;
		}

		if (checkCollision(m_projectiles[i], i))
		{
			removeProjectile(i);
			i--;
			continue;
		}

		m_projectiles[i].m_position.x += m_projectiles[i].m_direction.x * PROJECTILE_SPEED * deltaTime;
		m_projectiles[i].m_position.y += m_projectiles[i].m_direction.y * PROJECTILE_SPEED * deltaTime;

		if (m_projectiles[i].m_position.x < 0.0f || m_projectiles[i].m_position.x >= WORLD_SIZE
			|| m_projectiles[i].m_position.y < 0.0f || m_projectiles[i].m_position.y >= WORLD_SIZE)
		{
			removeProjectile(i);
			i--;
			continue;
		}

		auto updateProjectilePositionMessage = std::make_shared<message::UpdateProjectilePositionMessage>();
		updateProjectilePositionMessage->m_projectileId = m_projectiles[i].m_id;
		updateProjectilePositionMessage->m_positionX = m_projectiles[i].m_position.x;
		updateProjectilePositionMessage->m_positionY = m_projectiles[i].m_position.y;

		for (auto& [entity] : queryResult)
		{
			server::Server::Get()->sendTcpMessage(entity, updateProjectilePositionMessage);
		}
	}
}

void ProjectileManager::removeProjectile(size_t _index)
{
	auto removeProjectileMessage = std::make_shared<message::DeleteProjectileMessage>();
	removeProjectileMessage->m_projectileId = m_projectiles[_index].m_id;

	for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<tags::ConnectedTag, server::tags::ClientIsReadyTag, PlayerIsInitialized>{}))
	{
		server::Server::Get()->sendTcpMessage(entity, removeProjectileMessage);
	}

	m_projectiles.erase(m_projectiles.begin() + _index);
}

bool ProjectileManager::checkCollision(const Projectile& _projectil, const size_t _projectilIndex)
{
	for (size_t i = _projectilIndex + 1; i < m_projectiles.size(); i++)
	{
		float dx = _projectil.m_position.x - m_projectiles[i].m_position.x;
		float dy = _projectil.m_position.y - m_projectiles[i].m_position.y;

		float distanceSquared = dx * dx + dy * dy;
		float radiusSum = PROJECTILE_RADIUS + PROJECTILE_RADIUS;

		if (distanceSquared <= radiusSum * radiusSum)
		{
			return true;
		}
	}

	for (auto& player : PlayerManager::getPlayers())
	{
		if (player.m_entity == _projectil.m_shooter)
		{
			continue;
		}

		float dx = _projectil.m_position.x - player.m_position.x;
		float dy = _projectil.m_position.y - player.m_position.y;

		float distanceSquared = dx * dx + dy * dy;
		float radiusSum = PROJECTILE_RADIUS + PLAYER_RADIUS;

		if (distanceSquared <= radiusSum * radiusSum)
		{
			PlayerManager::playerHitByProjectile(player.m_entity);
			return true;
		}
	}

	return false;
}
