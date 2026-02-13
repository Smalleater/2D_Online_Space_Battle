#include <iostream>
#include <chrono>
#include <thread>

#include <TRA/netcode/server/server.hpp>

#include "player.hpp"
#include "projectile.hpp"

using namespace tra::netcode::server;

int main()
{
	srand(static_cast<unsigned int>(time(nullptr)));

	std::string port;
	uint8_t tickRate;

	std::cout << "Enter port number: ";
	std::cin >> port;

	std::cout << "Enter tickRate (max: 255): ";
	std::cin >> tickRate;

	uint16_t portNumber = static_cast<uint16_t>(std::stoi(port));
	Server::Get()->start(portNumber);

	std::vector<Player> players;

	while (Server::Get()->isRunning())
	{
		Server::Get()->updateElapsedTime();
		while (Server::Get()->canUpdateNetcode())
		{
			Server::Get()->beginUpdate();

			float fixedDeltaTime = Server::Get()->getFixedDeltaTime();

			PlayerManager::removeDisconnectedPlayers();
			PlayerManager::addNewPlayers();

			PlayerManager::updatePlayers(fixedDeltaTime);
			ProjectileManager::updateProjectiles(fixedDeltaTime);

			Server::Get()->endUpdate();
		}
	}
}
