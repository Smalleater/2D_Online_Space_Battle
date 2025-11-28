#include "player.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "TME/engine/newConnectionComponent.hpp"
#include "TME/engine/networkRootComponentTag.hpp"
#include "TME/engine/connectionStatusComponent.hpp"
#include "TME/engine/disconnectedComponent.hpp"

#include "gameMessage.hpp"

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
	updatePlayerRotation();
}

void PlayerManager::updatePlayerRotation()
{
	engine::EntityId selfEntityId = server::Server::Get()->getSelfEntityId();
	std::shared_ptr<engine::UpdateRotationMessage> updateRotationMessage = nullptr;

	std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
	for (size_t i = 0; i < playersId.size(); i++)
	{
		bool rotationUpdated = false;

		std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> messagesResult =
			server::Server::Get()->getTcpMessages(playersId[i], "RotationInputMessage");
		if (messagesResult.second.size() == 0)
		{
			continue;
		}

		auto it = std::find_if(m_players.begin(), m_players.end(),
			[playersId, i](const Player& player)
			{
				return player.m_id == playersId[i];
			});

		if (it == m_players.end())
		{
			continue;
		}

		for (size_t y = 0; y < messagesResult.second.size(); y++)
		{
			std::shared_ptr<engine::RotationInputMessage> rotationMessage =
				std::dynamic_pointer_cast<engine::RotationInputMessage>(messagesResult.second[y]);
			if (!rotationMessage)
			{
				continue;
			}

			float deltaX = rotationMessage->m_mouseWorldPosX - it->m_x;
			float deltaY = rotationMessage->m_mouseWorldPosY - it->m_y;
			float angleRadians = std::atan2(deltaY, deltaX);
			float angleDegrees = angleRadians * 180.0f / static_cast<float>(M_PI) + 90.0f;
			it->m_rotation = angleDegrees;

			rotationUpdated = true;
		}

		if (!rotationUpdated)
		{
			continue;
		}

		updateRotationMessage = std::make_shared<engine::UpdateRotationMessage>();
		updateRotationMessage->m_id = it->m_id;
		updateRotationMessage->m_rotation = it->m_rotation;
		for (size_t i = 0; i < playersId.size(); i++)
		{
			if (playersId[i] == selfEntityId || playersId[i] == it->m_id) continue;
			server::Server::Get()->sendTcpMessage(playersId[i], updateRotationMessage);
		}
	}
}