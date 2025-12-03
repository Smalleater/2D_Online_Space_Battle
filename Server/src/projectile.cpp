#include "projectile.hpp"

#include <TRA/server/server.hpp>
#include <TRA/engine/networkRootComponentTag.hpp>
#include <TRA/engine/connectionStatusComponent.hpp>

#include "gameMessage.hpp"
#include "player.hpp"

constexpr float WORLD_SIZE = 800.0f;
constexpr float PROJECTILE_LIFETIME = 1.0f;
constexpr float PROJECTILE_RADIUS = 5.0f;
constexpr float PROJECTILE_SPEED = 500.0f;

std::vector<Projectile> ProjectileManager::m_projectiles;
uint32_t ProjectileManager::m_nextProjectileId = 0;

using namespace tra;

void ProjectileManager::createProjectile(const tra::engine::EntityId _m_shooterId, const Vector2f& position, const Vector2f& direction)
{
	Projectile projectile;
	projectile.m_id = m_nextProjectileId++;
	projectile.m_shooterId = _m_shooterId;
	projectile.m_position = position;
	projectile.m_direction = direction;
	projectile.lifetime = 0.0f;
	m_projectiles.push_back(projectile);

	std::shared_ptr<engine::NewProjectileMessage> msg = std::make_shared<engine::NewProjectileMessage>();
	msg->m_projectileId = projectile.m_id;
	msg->m_positionX = projectile.m_position.x;
	msg->m_positionY = projectile.m_position.y;
	msg->m_directionX = projectile.m_direction.x;
	msg->m_directionY = projectile.m_direction.y;

	std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
	for (size_t i = 0; i < playersId.size(); i++)
	{
		server::Server::Get()->sendTcpMessage(playersId[i], msg);
	}
}

void ProjectileManager::updateProjectiles(const float deltaTime)
{
	std::shared_ptr<engine::UpdateProjectilePositionMessage> updateProjectilePositionMessage = nullptr;
	std::shared_ptr<engine::DeleteProjectileMessage> removeProjectileMessage = nullptr;
	std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();

	for (size_t i = 0; i < m_projectiles.size(); i++)
	{
		m_projectiles[i].lifetime += deltaTime;
		if (m_projectiles[i].lifetime >= PROJECTILE_LIFETIME)
		{
			removeProjectile(i);
			i--;

			continue;
		}

		if (checkCollision(m_projectiles[i]))
		{
			removeProjectile(i);
			i--;

			continue;
		}

		m_projectiles[i].m_position.x += m_projectiles[i].m_direction.x * PROJECTILE_SPEED * deltaTime;
		m_projectiles[i].m_position.y += m_projectiles[i].m_direction.y * PROJECTILE_SPEED * deltaTime;

		if (m_projectiles[i].m_position.x < 0.0f)
		{
			m_projectiles[i].m_position.x += WORLD_SIZE;
		}
		else if (m_projectiles[i].m_position.x >= WORLD_SIZE)
		{
			m_projectiles[i].m_position.x -= WORLD_SIZE;
		}

		if (m_projectiles[i].m_position.y < 0.0f)
		{
			m_projectiles[i].m_position.y += WORLD_SIZE;
		}
		else if (m_projectiles[i].m_position.y >= WORLD_SIZE)
		{
			m_projectiles[i].m_position.y -= WORLD_SIZE;
		}

		updateProjectilePositionMessage = std::make_shared<engine::UpdateProjectilePositionMessage>();
		updateProjectilePositionMessage->m_projectileId = m_projectiles[i].m_id;
		updateProjectilePositionMessage->m_positionX = m_projectiles[i].m_position.x;
		updateProjectilePositionMessage->m_positionY = m_projectiles[i].m_position.y;

		for (size_t j = 0; j < playersId.size(); j++)
		{
			server::Server::Get()->sendTcpMessage(playersId[j], updateProjectilePositionMessage);
		}
	}
}

void ProjectileManager::removeProjectile(size_t _index)
{
	std::shared_ptr<engine::DeleteProjectileMessage> removeProjectileMessage = std::make_shared<engine::DeleteProjectileMessage>();
	removeProjectileMessage->m_projectileId = m_projectiles[_index].m_id;

	std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
	for (size_t i = 0; i < playersId.size(); i++)
	{
		server::Server::Get()->sendTcpMessage(playersId[i], removeProjectileMessage);
	}

	m_projectiles.erase(m_projectiles.begin() + _index);
}

bool ProjectileManager::checkCollision(const Projectile& _projectil)
{
	for (auto& projectile : m_projectiles)
	{
		if (projectile.m_id == _projectil.m_id)
		{
			continue;
		}

		float dx = _projectil.m_position.x - projectile.m_position.x;
		float dy = _projectil.m_position.y - projectile.m_position.y;

		float distanceSquared = dx * dx + dy * dy;
		float radiusSum = PROJECTILE_RADIUS + PROJECTILE_RADIUS;

		if (distanceSquared <= radiusSum * radiusSum)
		{
			return true;
		}
	}

	for (auto& player : PlayerManager::getPlayers())
	{
		if (player.m_id == _projectil.m_shooterId)
		{
			continue;
		}

		float dx = _projectil.m_position.x - player.m_position.x;
		float dy = _projectil.m_position.y - player.m_position.y;

		float distanceSquared = dx * dx + dy * dy;
		float radiusSum = PROJECTILE_RADIUS + PLAYER_RADIUS;

		if (distanceSquared <= radiusSum * radiusSum)
		{
			PlayerManager::playerHitByProjectile(player.m_id);
			return true;
		}
	}

	return false;
}
