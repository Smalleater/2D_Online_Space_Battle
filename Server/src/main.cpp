#include <iostream>

#include "TME/server/server.hpp"

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

		PlayerManager::removeDisconnectedPlayers();
		PlayerManager::addNewPlayers();

		PlayerManager::updatePlayers(0.016f);

		server::Server::Get()->endUpdate();
	}
}
