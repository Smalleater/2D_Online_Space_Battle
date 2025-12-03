#include "projectile.hpp"

#include <TRA/server/server.hpp>
#include <TRA/engine/networkRootComponentTag.hpp>
#include <TRA/engine/connectionStatusComponent.hpp>

#include "gameMessage.hpp"

std::vector<Projectile> ProjectileManager::m_projectiles;
uint32_t ProjectileManager::m_nextProjectileId = 0;

using namespace tra;

void ProjectileManager::createProjectile(const Vector2f& position, const Vector2f& direction)
{
	Projectile projectile;
	projectile.m_id = m_nextProjectileId++;
	projectile.m_position = position;
	projectile.m_direction = direction;
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
