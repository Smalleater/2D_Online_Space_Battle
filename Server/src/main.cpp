#include <iostream>

#include "TME/server/server.hpp"
#include "TME/engine/newConnectionComponent.hpp"
#include "TME/engine/networkRootComponentTag.hpp"
#include "TME/engine/connectionStatusComponent.hpp"

#include "gameMessage.hpp"
#include "player.hpp"

using namespace tme;

int main()
{
	std::string port;

	std::cout << "Enter port number to start the server: ";
	std::cin >> port;

	uint16_t portNumber = static_cast<uint16_t>(std::stoi(port));
	server::Server::Get()->Start(portNumber);

	engine::EntityId selfEntityId = server::Server::Get()->getSelfEntityId();

	std::vector<Player> players;
	while (server::Server::Get()->isRunning())
	{
		server::Server::Get()->beginUpdate();

		std::vector<engine::EntityId> newConections = server::Server::Get()->queryEntityIds<engine::NewConnectionComponentTag>();
		for (size_t i = 0; i < newConections.size(); i++)
		{
			Player newPlayer;
			newPlayer.m_id = newConections[i];
			players.push_back(newPlayer);

			std::vector<engine::EntityId> playersId = server::Server::Get()->queryEntityIds<engine::NetworkRootComponentTag, engine::ConnectedComponentTag>();
			std::shared_ptr<engine::NewClientMessage> newClientMessage = nullptr;
			for (size_t y = 0; y < playersId.size(); y++)
			{
				if (playersId[y] == selfEntityId
					|| playersId[y] == newConections[i]) continue;

				newClientMessage = std::make_shared<engine::NewClientMessage>();
				newClientMessage->m_id = newConections[i];
				server::Server::Get()->sendTcpMessage(playersId[y], newClientMessage);
			}
		}

		server::Server::Get()->endUpdate();
	}
}
