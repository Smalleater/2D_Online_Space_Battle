#include "player.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "TME/engine/newConnectionComponent.hpp"
#include "TME/engine/networkRootComponentTag.hpp"
#include "TME/engine/connectionStatusComponent.hpp"
#include "TME/engine/disconnectedComponent.hpp"

#include "gameMessage.hpp"

constexpr const float MoveSpeed = 200.0f;
constexpr const float WorldSize = 800.0f;

using namespace tme;

std::vector<Player> PlayerManager::m_players;

void PlayerManager::removeDisconnectedPlayers()
{
	engine::EntityId selfEntityId = server::Server::Get()->getSelfEntityId();

	std::vector<engine::EntityId> newConections = server::Server::Get()->queryEntityIds<engine::NewConnectionComponentTag>();
	for (size_t i = 0; i < newConections.size(); i++)
	{
		Player newPlayer;
		newPlayer.m_id = newConections[i];
		m_players.push_back(newPlayer);

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

void PlayerManager::addNewPlayers()
{
	engine::EntityId selfEntityId = server::Server::Get()->getSelfEntityId();

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
			if (playersId[y] == selfEntityId || playersId[y] == it->m_id) continue;
			disconnectMessage = std::make_shared<engine::DisconnectedClientMessage>();
			disconnectMessage->m_id = it->m_id;
			server::Server::Get()->sendTcpMessage(playersId[y], disconnectMessage);
		}

		m_players.erase(it);
	}
}

void PlayerManager::updatePlayers(float deltaTime)
{
	engine::EntityId selfEntityId = server::Server::Get()->getSelfEntityId();
	std::shared_ptr<engine::UpdateRotationAndPositionMessage> updateRotationAndPositionMessage = nullptr;

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

		std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> messagesResult =
			server::Server::Get()->getTcpMessages(playersId[i], "MovementInputMessage");
		if (messagesResult.second.size() == 0)
		{
			continue;
		}

		std::shared_ptr<engine::MovementInputMessage> movementMessage =
			std::dynamic_pointer_cast<engine::MovementInputMessage>(messagesResult.second[messagesResult.second.size() - 1]);
		if (!movementMessage)
		{
			continue;
		}

		updatePlayerRotation(movementMessage->m_mouseWorldPosX, movementMessage->m_mouseWorldPosY, it);
		updatePlayerMovement(movementMessage->m_moveDirectionX, movementMessage->m_moveDirectionY, it, deltaTime);

		updateRotationAndPositionMessage = std::make_shared<engine::UpdateRotationAndPositionMessage>();
		updateRotationAndPositionMessage->m_id = it->m_id;
		updateRotationAndPositionMessage->m_rotation = it->m_rotation;
		updateRotationAndPositionMessage->m_positionX = it->m_position.x;
		updateRotationAndPositionMessage->m_positionY = it->m_position.y;
		for (size_t i = 0; i < playersId.size(); i++)
		{
			if (playersId[i] == selfEntityId || playersId[i] == it->m_id) continue;
			server::Server::Get()->sendTcpMessage(playersId[i], updateRotationAndPositionMessage);
		}
	}
}

void PlayerManager::updatePlayerRotation(const float _mouseWorldPosX, const float _mouseWorldPosY, std::vector<Player>::iterator& _it)
{
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
	movement += forwardDirection * static_cast<float>(_moveDirectionX) * MoveSpeed * deltaTime;

	_it->m_position += movement;

	if (_it->m_position.x < 0.0f)
	{
		_it->m_position.x += WorldSize;
	}
	else if (_it->m_position.x >= WorldSize)
	{
		_it->m_position.x -= WorldSize;
	}

	if (_it->m_position.y < 0.0f)
	{
		_it->m_position.y += WorldSize;
	}
	else if (_it->m_position.y >= WorldSize)
	{
		_it->m_position.y -= WorldSize;
	}
}