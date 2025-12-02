#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

#include "gameMessage.hpp"

class Player
{
public:
	Player();
	~Player();

	void PollEvents(const std::optional<sf::Event>& _event, const float _dt);

	void Update(const float _dt, const sf::RenderWindow& _window);

	void Draw(sf::RenderWindow& _window);

private:
	sf::Texture m_spriteTexture;
	sf::Sprite* m_sprite;

	sf::Vector2i m_moveDirection;
	sf::Vector2f m_lastWorldMousePosition;

	bool m_isNotFocused = false;
	bool m_isShooting = false;

	void UpdateRotation(std::shared_ptr<tra::engine::MovementInputMessage> _movementInputMessage, const float _dt, const sf::RenderWindow& _window);
	void Move(std::shared_ptr<tra::engine::MovementInputMessage> _movementInputMessage, const float _dt);
};

#endif
