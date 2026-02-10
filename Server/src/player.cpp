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
std::map<ecs::Entity, size_t> PlayerManager::m_playersSparse;

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

		std::shared_ptr<message::RespawnMessage> respawnMessage = std::make_shared<message::RespawnMessage>();
		respawnMessage->m_positionX = newPlayer.m_position.x;
		respawnMessage->m_positionY = newPlayer.m_position.y;
		server::Server::Get()->sendTcpMessage(newConnectionEntity, respawnMessage);

		auto newClientMessage = std::make_shared<message::NewClientMessage>();

		for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
			ecs::WithComponent<>{},
			ecs::WithoutComponent<>{},
			ecs::WithTag<tags::ConnectedTag>{}))
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
	for (auto& [entity] : server::Server::Get()->getEcsWorld()->queryEntities(
		ecs::WithComponent<>{},
		ecs::WithoutComponent<>{},
		ecs::WithTag<tags::ConnectedTag>{}))
	{

	}

	/////////////////////////////

	std::shared_ptr<engine::UpdateRotationAndPositionMessage> updateRotationAndPositionMessage = nullptr;

	std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> messagesResult;

	std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
	for (size_t i = 0; i < playersId.size(); i++)
	{
		auto it = std::find_if(m_players.begin(), m_players.end(),
			[playersId, i](const Player& player)
			{
				return player.m_id == playersId[i];
			});

		if (it == m_players.end())
		{
			continue;
		}

		messagesResult = server::Server::Get()->getTcpMessages(playersId[i], "MovementInputMessage");
		if (messagesResult.second.size() != 0)
		{
			std::shared_ptr<engine::MovementInputMessage> movementMessage =
				std::dynamic_pointer_cast<engine::MovementInputMessage>(messagesResult.second[messagesResult.second.size() - 1]);
			if (movementMessage)
			{
				updatePlayerRotation(movementMessage->m_mouseWorldPosX, movementMessage->m_mouseWorldPosY, it);
				updatePlayerMovement(movementMessage->m_moveDirectionX, movementMessage->m_moveDirectionY, it, deltaTime);

				updateRotationAndPositionMessage = std::make_shared<engine::UpdateRotationAndPositionMessage>();
				updateRotationAndPositionMessage->m_id = it->m_id;
				updateRotationAndPositionMessage->m_rotation = it->m_rotation;
				updateRotationAndPositionMessage->m_positionX = it->m_position.x;
				updateRotationAndPositionMessage->m_positionY = it->m_position.y;
				for (size_t i = 0; i < playersId.size(); i++)
				{
					if (playersId[i] != it->m_id)
					{
						server::Server::Get()->sendTcpMessage(playersId[i], updateRotationAndPositionMessage);
					}
				}
			}
		}

		messagesResult = server::Server::Get()->getTcpMessages(playersId[i], "ShootInputMessage");
		if (messagesResult.second.size() != 0)
		{
			if (it->m_shootCooldown <= 0)
			{
				it->m_shootCooldown = SHOOT_COOLDOWN;
				Vector2f direction(std::cos(it->m_rotation - M_PI / 2), std::sin(it->m_rotation - M_PI / 2));
				ProjectileManager::createProjectile(it->m_id, it->m_position, direction);
			}
		}

		if (it->m_shootCooldown > 0)
		{
			it->m_shootCooldown -= deltaTime;
		}
	}
}

void PlayerManager::playerHitByProjectile(const tra::ecs::Entity _playerEntity)
{
	auto it = std::find_if(m_players.begin(), m_players.end(),
		[_playerEntity](const Player& player)
		{
			return player.m_id == _playerEntity;
		});
	if (it == m_players.end())
	{
		return;
	}

	it->m_position = getRespawnPosition();

	std::shared_ptr<engine::RespawnMessage> respawnMessage = std::make_shared<engine::RespawnMessage>();
	respawnMessage->m_positionX = it->m_position.x;
	respawnMessage->m_positionY = it->m_position.y;
	server::Server::Get()->sendTcpMessage(it->m_id, respawnMessage);

	std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
	std::shared_ptr<engine::UpdateRotationAndPositionMessage> updateRotationAndPositionMessage = std::make_shared<engine::UpdateRotationAndPositionMessage>();

	updateRotationAndPositionMessage->m_id = it->m_id;
	updateRotationAndPositionMessage->m_rotation = it->m_rotation;
	updateRotationAndPositionMessage->m_positionX = it->m_position.x;
	updateRotationAndPositionMessage->m_positionY = it->m_position.y;

	for (size_t i = 0; i < playersId.size(); i++)
	{
		if (playersId[i] != it->m_id)
		{
			server::Server::Get()->sendTcpMessage(playersId[i], updateRotationAndPositionMessage);
		}
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

void PlayerManager::updatePlayerRotation(const float _mouseWorldPosX, const float _mouseWorldPosY, std::vector<Player>::iterator& _it)
{
	_it->m_lastMousePosition = Vector2f(_mouseWorldPosX, _mouseWorldPosY);

	float deltaX = _mouseWorldPosX - _it->m_position.x;
	float deltaY = _mouseWorldPosY - _it->m_position.y;
	float angleRadians = std::atan2(deltaY, deltaX) + 90.0f * (static_cast<float>(M_PI) / 180.0f);
	_it->m_rotation = angleRadians;
}

void PlayerManager::updatePlayerMovement(const int _moveDirectionX, const int _moveDirectionY, std::vector<Player>::iterator& _it, const float deltaTime)
{
	float rotation = _it->m_rotation;

	Vector2f forwardDirection(std::cos(rotation), std::sin(rotation));
	Vector2f rightDirection(-std::sin(rotation), std::cos(rotation));

	Vector2f movement(0.0f, 0.0f);
	movement += forwardDirection * static_cast<float>(_moveDirectionX) * MOVE_SPEED * deltaTime;
	movement += rightDirection * static_cast<float>(_moveDirectionY) * MOVE_SPEED * deltaTime;

	_it->m_position += movement;

	float shipToMouseDistance = std::sqrt((_it->m_lastMousePosition.x - _it->m_position.x)
		* (_it->m_lastMousePosition.x - _it->m_position.x)
		+ (_it->m_lastMousePosition.y - _it->m_position.y)
		* (_it->m_lastMousePosition.y - _it->m_position.y));

	if (shipToMouseDistance < MOUSE_DEAD_ZONE)
	{
		_it->m_position -= movement;
		return;
	}

	if (_it->m_position.x < 0.0f)
	{
		_it->m_position.x += WORLD_SIZE;
	}
	else if (_it->m_position.x >= WORLD_SIZE)
	{
		_it->m_position.x -= WORLD_SIZE;
	}

	if (_it->m_position.y < 0.0f)
	{
		_it->m_position.y += WORLD_SIZE;
	}
	else if (_it->m_position.y >= WORLD_SIZE)
	{
		_it->m_position.y -= WORLD_SIZE;
	}
}