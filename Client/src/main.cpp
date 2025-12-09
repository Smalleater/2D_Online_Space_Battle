#include <SFML/Graphics.hpp>
#include <TRA/client/client.hpp>

#include "player.hpp"
#include "enemy.hpp"
#include "projectile.hpp"

constexpr bool EnableBot = false;

using namespace tra;

int main()
{
	std::string address = "127.0.0.1";
	uint16_t port = 20250;

	/*std::cout << "Enter connection address: ";
	std::cin >> address;
	std::cout << "Enter connection port: ";
	std::cin >> port;*/

	client::Client::Get()->ConnectTo(address, port);

	Player player;
	player.SetIsBot(EnableBot);

	auto window = sf::RenderWindow(sf::VideoMode({ 800u, 800u }), "2D Online Space Battle");
	window.setFramerateLimit(75);
	window.setKeyRepeatEnabled(false);

	sf::Clock clock;
	while (window.isOpen() && client::Client::Get()->IsConnected())
	{
		float deltaTime = clock.restart().asSeconds();

		client::Client::Get()->beginUpdate();

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

			player.PollEvents(event);
		}

		player.Update(deltaTime, window);
		ProjectileManager::UpdateProjectiles(deltaTime);
		EnemyManager::UpdateEnemis();

		client::Client::Get()->endUpdate();

		window.clear();
		if (!EnableBot)
		{
			ProjectileManager::DrawProjectiles(window);
			EnemyManager::DrawEnemis(window);
			player.Draw(window);
		}
		window.display();
	}

	if (client::Client::Get()->IsConnected())
	{
		client::Client::Get()->Disconnect();
	}
}
