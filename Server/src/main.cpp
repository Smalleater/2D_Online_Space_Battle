#include <iostream>
#include <chrono>
#include <thread>

#include "TRA/server/server.hpp"

#include "player.hpp"

using namespace tra;

int main()
{
	std::string port;

	std::cout << "Enter port number to start the server: ";
	std::cin >> port;

	uint16_t portNumber = static_cast<uint16_t>(std::stoi(port));
	server::Server::Get()->Start(portNumber);

	engine::EntityId selfEntityId = server::Server::Get()->getSelfEntityId();

	std::vector<Player> players;

	const int targetFPS = 60;
	const auto frameDuration = std::chrono::milliseconds(1000 / targetFPS);
	std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
	while (server::Server::Get()->isRunning())
	{
		auto start = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(start - lastFrameTime).count();
		lastFrameTime = start;

		server::Server::Get()->beginUpdate();

		PlayerManager::removeDisconnectedPlayers();
		PlayerManager::addNewPlayers();

		PlayerManager::updatePlayers(deltaTime);

		server::Server::Get()->endUpdate();

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = end - start;

		if (elapsed < frameDuration) 
		{
			std::this_thread::sleep_for(frameDuration - elapsed);
		}
	}
}
