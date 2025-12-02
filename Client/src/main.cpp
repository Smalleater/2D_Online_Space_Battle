#include <SFML/Graphics.hpp>

#include "TRA/client/client.hpp"

#include "player.hpp"
#include "enemy.hpp"

using namespace tra;

int main()
{
	std::string address;
	uint16_t port;

	std::cout << "Enter connection address: ";
	std::cin >> address;
	std::cout << "Enter connection port: ";
	std::cin >> port;

	client::Client::Get()->ConnectTo(address, port);

	Player player;

    auto window = sf::RenderWindow(sf::VideoMode({800u, 800u}), "2D Online Space Battle");
    window.setFramerateLimit(144);
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

			player.PollEvents(event, deltaTime);
        }

		player.Update(deltaTime, window);

		client::Client::Get()->endUpdate();
		EnemyManager::UpdateEnemis();

        window.clear();
		EnemyManager::DrawEnemis(window);
		player.Draw(window);
        window.display();
    }

    if (client::Client::Get()->IsConnected())
    {
		client::Client::Get()->Disconnect();
    }
}
