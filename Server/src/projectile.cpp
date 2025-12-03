#include "projectile.hpp"

#include <TRA/server/server.hpp>
#include <TRA/engine/networkRootComponentTag.hpp>
#include <TRA/engine/connectionStatusComponent.hpp>

#include "gameMessage.hpp"

constexpr float WORLD_SIZE = 800.0f;
constexpr float PROJECTILE_LIFETIME = 2.0f;

std::vector<Projectile> ProjectileManager::m_projectiles;
uint32_t ProjectileManager::m_nextProjectileId = 0;

using namespace tra;

void ProjectileManager::createProjectile(const Vector2f& position, const Vector2f& direction)
{
	Projectile projectile;
	projectile.m_id = m_nextProjectileId++;
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

	const float speed = 300.0f;
	for (size_t i = 0; i < m_projectiles.size(); i++)
	{
		m_projectiles[i].lifetime += deltaTime;
		if (m_projectiles[i].lifetime >= PROJECTILE_LIFETIME)
		{
			removeProjectileMessage = std::make_shared<engine::DeleteProjectileMessage>();
			removeProjectileMessage->m_projectileId = m_projectiles[i].m_id;

			for (size_t j = 0; j < playersId.size(); j++)
			{
				server::Server::Get()->sendTcpMessage(playersId[j], removeProjectileMessage);
			}

			m_projectiles.erase(m_projectiles.begin() + i);
			i--;

			continue;
		}

		m_projectiles[i].m_position.x += m_projectiles[i].m_direction.x * speed * deltaTime;
		m_projectiles[i].m_position.y += m_projectiles[i].m_direction.y * speed * deltaTime;

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
