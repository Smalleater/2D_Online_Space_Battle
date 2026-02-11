#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <random>

#include "gameMessage.hpp"

class Player
{
public:
	Player();
	~Player();

	void PollEvents(const std::optional<sf::Event>& _event);
	void BotEvents(const float _dt);

	void Update(const float _dt, const sf::RenderWindow& _window);

	void Draw(sf::RenderWindow& _window);

	void SetIsBot(bool _isBot) { m_isBot = _isBot; }
	bool IsBot() const { return m_isBot; }

private:
	sf::Texture m_spriteTexture;
	sf::Sprite* m_sprite;

	sf::Vector2i m_moveDirection;
	sf::Vector2f m_lastWorldMousePosition;

	bool m_isNotFocused = false;
	bool m_isShooting = false;

	bool m_isBot = false;
	std::mt19937 m_rng;
	sf::Vector2f m_botTarget;
	float m_botChangeTimer = 0.0f;
	float m_shootCooldown = 0.0f;

	void UpdateRotation(std::shared_ptr<tra::message::MovementInputMessage> _movementInputMessage, const float _dt, const sf::RenderWindow& _window);
	void Move(std::shared_ptr<tra::message::MovementInputMessage> _movementInputMessage, const float _dt);
};

#endif
