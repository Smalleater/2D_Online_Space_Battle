#include <SFML/Graphics.hpp>
#include <TRA/netcode/client/client.hpp>

#include "player.hpp"
#include "enemy.hpp"
#include "projectile.hpp"

constexpr bool EnableBot = false;

using namespace tra;
using namespace tra::netcode::client;

int main()
{
	std::string address = "127.0.0.1";
	uint16_t port = 20250;

	/*std::cout << "Enter connection address: ";
	std::cin >> address;
	std::cout << "Enter connection port: ";
	std::cin >> port;*/

	Client::Get()->connectTo(address, port);

	Player player;
	player.SetIsBot(EnableBot);

	auto window = sf::RenderWindow(sf::VideoMode({ 800u, 800u }), "2D Online Space Battle");
	window.setFramerateLimit(75);
	window.setKeyRepeatEnabled(false);

	sf::Clock clock;
	while (window.isOpen() && Client::Get()->isConnected())
	{
		float deltaTime = clock.restart().asSeconds();

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

			player.PollEvents(event);
		}

		std::cout << Client::Get()->getCurrentTick() << std::endl;

		Client::Get()->updateElapsedTime();
		while (Client::Get()->canUpdateNetcode())
		{
			std::cout << "Update netcode" << std::endl;

			Client::Get()->beginUpdate();

			if (Client::Get()->isReady())
			{
				std::cout << "Is ready" << std::endl;

				float fixedDeltatime = Client::Get()->getFixedDeltaTime();

				player.Update(fixedDeltatime, window);
				ProjectileManager::UpdateProjectiles(fixedDeltatime);
				EnemyManager::UpdateEnemis();
			}

			Client::Get()->endUpdate();
		}

		window.clear();
		if (!EnableBot)
		{
			ProjectileManager::DrawProjectiles(window);
			EnemyManager::DrawEnemis(window);
			player.Draw(window);
		}
		window.display();
	}
}
