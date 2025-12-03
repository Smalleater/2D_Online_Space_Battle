#include "player.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <TRA/engine/newConnectionComponent.hpp>
#include <TRA/engine/networkRootComponentTag.hpp>
#include <TRA/engine/connectionStatusComponent.hpp>
#include <TRA/engine/disconnectedComponent.hpp>

#include "gameMessage.hpp"
#include "projectile.hpp"

constexpr float MOVE_SPEED = 200.0f;
constexpr float WORLD_SIZE = 800.0f;
constexpr float MOUSE_DEAD_ZONE = 5.0f;
constexpr float SHOOT_COOLDOWN = 0.3f;
constexpr float RESPAWN_BORDER_OFFSET = 50.0f;

using namespace tra;

std::vector<Player> PlayerManager::m_players;

void PlayerManager::removeDisconnectedPlayers()
{
	std::vector<engine::EntityId> disconnectedClients = server::Server::Get()->queryEntityIds<engine::DisconnectedComponentTag>();
	for (size_t i = 0; i < disconnectedClients.size(); i++)
	{
		auto it = std::find_if(m_players.begin(), m_players.end(),
			[disconnectedClients, i](const Player& player)
			{
				return player.m_id == disconnectedClients[i];
			});

		if (it == m_players.end())
		{
			continue;
		}

		std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
		std::shared_ptr<engine::DisconnectedClientMessage> disconnectMessage = nullptr;
		for (size_t y = 0; y < playersId.size(); y++)
		{
			if (playersId[y] == it->m_id) continue;
			disconnectMessage = std::make_shared<engine::DisconnectedClientMessage>();
			disconnectMessage->m_id = it->m_id;
			server::Server::Get()->sendTcpMessage(playersId[y], disconnectMessage);
		}

		m_players.erase(it);
	}
}

void PlayerManager::addNewPlayers()
{
	engine::EntityId selfEntityId = server::Server::Get()->getSelfEntityId();

	std::vector<engine::EntityId> newConections = server::Server::Get()->queryEntityIds<engine::NewConnectionComponentTag>();
	for (size_t i = 0; i < newConections.size(); i++)
	{
		Player newPlayer;
		newPlayer.m_id = newConections[i];
		newPlayer.m_position = getRespawnPosition();
		newPlayer.m_lastMousePosition = Vector2f(0, 0);
		newPlayer.m_rotation = 0.0f;
		newPlayer.m_shootCooldown = 0.0f;
		m_players.push_back(newPlayer);

		std::shared_ptr<engine::RespawnMessage> respawnMessage = std::make_shared<engine::RespawnMessage>();
		respawnMessage->m_positionX = newPlayer.m_position.x;
		respawnMessage->m_positionY = newPlayer.m_position.y;
		server::Server::Get()->sendTcpMessage(newPlayer.m_id, respawnMessage);

		std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
		std::shared_ptr<engine::NewClientMessage> newClientMessage = nullptr;
		for (size_t y = 0; y < playersId.size(); y++)
		{
			if (playersId[y] == selfEntityId || playersId[y] == newConections[i]) continue;

			newClientMessage = std::make_shared<engine::NewClientMessage>();
			newClientMessage->m_id = newConections[i];

			server::Server::Get()->sendTcpMessage(playersId[y], newClientMessage);
		}

		for (size_t y = 0; y < playersId.size(); y++)
		{
			if (playersId[y] == selfEntityId || playersId[y] == newConections[i]) continue;

			newClientMessage = std::make_shared<engine::NewClientMessage>();
			newClientMessage->m_id = playersId[y];

			server::Server::Get()->sendTcpMessage(newConections[i], newClientMessage);
		}
	}
}

void PlayerManager::updatePlayers(float deltaTime)
{
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

void PlayerManager::playerHitByProjectile(const tra::engine::EntityId _playerEntityId)
{
	auto it = std::find_if(m_players.begin(), m_players.end(),
		[_playerEntityId](const Player& player)
		{
			return player.m_id == _playerEntityId;
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