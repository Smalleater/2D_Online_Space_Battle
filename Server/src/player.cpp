#include "player.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "TRA/netcode/engine/tags.hpp"

#include "gameMessage.hpp"
#include "projectile.hpp"

constexpr float MOVE_SPEED = 200.0f;
constexpr float WORLD_SIZE = 800.0f;
constexpr float MOUSE_DEAD_ZONE = 5.0f;
constexpr float SHOOT_COOLDOWN = 0.3f;
constexpr float RESPAWN_BORDER_OFFSET = 50.0f;

using namespace tra;
using namespace tra::netcode;

std::vector<Player> PlayerManager::m_players;
std::unordered_map<ecs::Entity, size_t> PlayerManager::m_playersSparse;

void PlayerManager::removeDisconnectedPlayers()
{
	for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<tags::DisconnectedTag>{}))
	{
		auto it = m_playersSparse.find(entity);
		if (it == m_playersSparse.end())
		{
			continue;
		}

		size_t playerIndex = it->second;
		Player& player = m_players.at(playerIndex);

		auto disconnectedClientMessage = std::make_shared<message::DisconnectedClientMessage>();
		disconnectedClientMessage->m_id = entity.id();

		for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
			ecs::WithComponent<>{},
			ecs::WithoutComponent<>{},
			ecs::WithTag<tags::ConnectedTag>{}))
		{
			server::Server::Get()->sendTcpMessage(entity, disconnectedClientMessage);
		}

		size_t lastIndex = m_players.size() - 1;

		if (playerIndex != lastIndex)
		{
			m_players[playerIndex] = std::move(m_players[lastIndex]);
			m_playersSparse[m_players[playerIndex].m_entity] = playerIndex;
		}

		m_players.pop_back();
		m_playersSparse.erase(it);
	}
}

void PlayerManager::addNewPlayers()
{
	for (auto& [newConnectionEntity] : server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<tags::NewConnectionTag>{}))
	{
		Player newPlayer;
		newPlayer.m_entity = newConnectionEntity;
		newPlayer.m_position = getRespawnPosition();
		newPlayer.m_lastMousePosition = Vector2f(0, 0);
		newPlayer.m_rotation = 0.0f;
		newPlayer.m_shootCooldown = 0.0f;

		m_players.push_back(newPlayer);
		m_playersSparse.insert({ newConnectionEntity, m_players.size() - 1 });

		std::shared_ptr<message::RespawnMessage> respawnMessage = std::make_shared<message::RespawnMessage>();
		respawnMessage->m_positionX = newPlayer.m_position.x;
		respawnMessage->m_positionY = newPlayer.m_position.y;
		server::Server::Get()->sendTcpMessage(newConnectionEntity, respawnMessage);

		auto newClientMessage = std::make_shared<message::NewClientMessage>();
		newClientMessage->m_id = newConnectionEntity.id();

		for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
			ecs::WithComponent<>{},
			ecs::WithoutComponent<>{},
			ecs::WithTag<tags::ConnectedTag>{},
			ecs::WithoutTag<tags::NewConnectionTag>{}))
		{
			server::Server::Get()->sendTcpMessage(entity, newClientMessage);

			auto oldNewClientMessage = std::make_shared<message::NewClientMessage>();
			oldNewClientMessage->m_id = entity.id();
			server::Server::Get()->sendTcpMessage(newConnectionEntity, oldNewClientMessage);
		}
	}
}

void PlayerManager::updatePlayers(float deltaTime)
{
	auto& queryResult = server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<tags::ConnectedTag>{});

	for (auto& [entity] : queryResult)
	{
		auto it = m_playersSparse.find(entity);
		if (it == m_playersSparse.end())
		{
			continue;
		}

		Player& player = m_players.at(it->second);

		auto movementInputMessages = server::Server::Get()->getTcpMessages(entity, "MovementInputMessage");
		if (movementInputMessages.size() > 0)
		{
			auto movementMessage = std::dynamic_pointer_cast<message::MovementInputMessage>(movementInputMessages.back());

			updatePlayerRotation(movementMessage->m_mouseWorldPosX, movementMessage->m_mouseWorldPosY, player);
			updatePlayerMovement(movementMessage->m_moveDirectionX, movementMessage->m_moveDirectionY, player, deltaTime);

			auto updateRotationAndPositionMessage = std::make_shared<message::UpdateRotationAndPositionMessage>();

			updateRotationAndPositionMessage->m_id = player.m_entity.id();
			updateRotationAndPositionMessage->m_rotation = player.m_rotation;
			updateRotationAndPositionMessage->m_positionX = player.m_position.x;
			updateRotationAndPositionMessage->m_positionY = player.m_position.y;

			for (auto& [entityToSend] : queryResult)
			{
				if (entityToSend == entity)
				{
					continue;
				}

				server::Server::Get()->sendTcpMessage(entityToSend, updateRotationAndPositionMessage);
			}
		}

		auto shootInputMessages = server::Server::Get()->getTcpMessages(entity, "ShootInputMessage");
		if (shootInputMessages.size() > 0 && player.m_shootCooldown <= 0)
		{
			player.m_shootCooldown = SHOOT_COOLDOWN;
			Vector2f direction(std::cos(player.m_rotation - M_PI / 2), std::sin(player.m_rotation - M_PI / 2));
			ProjectileManager::createProjectile(player.m_entity, player.m_position, direction);
		}

		if (player.m_shootCooldown > 0)
		{
			player.m_shootCooldown -= deltaTime;
		}
	}
}

void PlayerManager::playerHitByProjectile(const tra::ecs::Entity _entity)
{
	auto it = m_playersSparse.find(_entity);
	if (it == m_playersSparse.end())
	{
		return;
	}

	Player& player = m_players.at(it->second);

	player.m_position = getRespawnPosition();

	auto respawnMessage = std::make_shared<message::RespawnMessage>();

	respawnMessage->m_positionX = player.m_position.x;
	respawnMessage->m_positionY = player.m_position.y;

	server::Server::Get()->sendTcpMessage(player.m_entity, respawnMessage);

	auto updateRotationAndPositionMessage = std::make_shared<message::UpdateRotationAndPositionMessage>();

	updateRotationAndPositionMessage->m_id = player.m_entity.id();
	updateRotationAndPositionMessage->m_rotation = player.m_rotation;
	updateRotationAndPositionMessage->m_positionX = player.m_position.x;
	updateRotationAndPositionMessage->m_positionY = player.m_position.y;

	for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<tags::ConnectedTag>{}))
	{
		if (entity == player.m_entity)
		{
			continue;
		}

		server::Server::Get()->sendTcpMessage(entity, updateRotationAndPositionMessage);
	}
}

Vector2f PlayerManager::getRespawnPosition()
{
	Vector2f respawnPosition;
	if (rand() % 2)
	{
		respawnPosition.x = rand() % 2 ? 0.0f + RESPAWN_BORDER_OFFSET : WORLD_SIZE - RESPAWN_BORDER_OFFSET;
		respawnPosition.y = RESPAWN_BORDER_OFFSET + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (WORLD_SIZE - 2 * RESPAWN_BORDER_OFFSET);
	}
	else
	{
		respawnPosition.x = RESPAWN_BORDER_OFFSET + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (WORLD_SIZE - 2 * RESPAWN_BORDER_OFFSET);
		respawnPosition.y = rand() % 2 ? 0.0f + RESPAWN_BORDER_OFFSET : WORLD_SIZE - RESPAWN_BORDER_OFFSET;
	}

	return respawnPosition;
}

void PlayerManager::updatePlayerRotation(const float _mouseWorldPosX, const float _mouseWorldPosY, Player& _player)
{
	_player.m_lastMousePosition = Vector2f(_mouseWorldPosX, _mouseWorldPosY);

	float deltaX = _mouseWorldPosX - _player.m_position.x;
	float deltaY = _mouseWorldPosY - _player.m_position.y;
	float angleRadians = std::atan2(deltaY, deltaX) + 90.0f * (static_cast<float>(M_PI) / 180.0f);
	_player.m_rotation = angleRadians;
}

void PlayerManager::updatePlayerMovement(const int _moveDirectionX, const int _moveDirectionY, Player& _player, const float deltaTime)
{
	float rotation = _player.m_rotation;

	Vector2f forwardDirection(std::cos(rotation), std::sin(rotation));
	Vector2f rightDirection(-std::sin(rotation), std::cos(rotation));

	Vector2f movement(0.0f, 0.0f);
	movement += forwardDirection * static_cast<float>(_moveDirectionX) * MOVE_SPEED * deltaTime;
	movement += rightDirection * static_cast<float>(_moveDirectionY) * MOVE_SPEED * deltaTime;

	_player.m_position += movement;

	float shipToMouseDistance = std::sqrt((_player.m_lastMousePosition.x - _player.m_position.x)
		* (_player.m_lastMousePosition.x - _player.m_position.x)
		+ (_player.m_lastMousePosition.y - _player.m_position.y)
		* (_player.m_lastMousePosition.y - _player.m_position.y));

	if (shipToMouseDistance < MOUSE_DEAD_ZONE)
	{
		_player.m_position -= movement;
		return;
	}

	if (_player.m_position.x < 0.0f)
	{
		_player.m_position.x += WORLD_SIZE;
	}
	else if (_player.m_position.x >= WORLD_SIZE)
	{
		_player.m_position.x -= WORLD_SIZE;
	}

	if (_player.m_position.y < 0.0f)
	{
		_player.m_position.y += WORLD_SIZE;
	}
	else if (_player.m_position.y >= WORLD_SIZE)
	{
		_player.m_position.y -= WORLD_SIZE;
	}
}