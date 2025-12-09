#include "player.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <TRA/client/client.hpp>

#include "gameMessage.hpp"

#define SPRITE_LOAD_PATH "resources/sprites/ship.png"

constexpr float MOVE_SPEED = 200.0f;
constexpr float WORLD_SIZE = 800.0f;
constexpr float MOUSE_DEAD_ZONE = 5.0f;

using namespace tra;

static inline int signf(float v) { return (v > 0.0f) - (v < 0.0f); }

Player::Player()
{
	std::random_device rd;
	m_rng.seed(rd());

	if (!m_spriteTexture.loadFromFile(SPRITE_LOAD_PATH))
	{
		throw std::runtime_error("Failed to load player sprite texture from " SPRITE_LOAD_PATH);
	}

	m_sprite = new sf::Sprite(m_spriteTexture);
	m_sprite->setOrigin(sf::Vector2f(m_spriteTexture.getSize().x / 2.0f, m_spriteTexture.getSize().y / 2.0f));
	
	m_moveDirection = sf::Vector2i(0, 0);
	m_lastWorldMousePosition = sf::Vector2f(0.0f, 0.0f);
	m_botTarget = sf::Vector2f(0.0f, 0.0f);
	m_botChangeTimer = 0.0f;
	m_shootCooldown = 0.0f;
}

Player::~Player()
{

}

void Player::PollEvents(const std::optional<sf::Event>& _event)
{
	if (m_isBot) return;

	if (const auto* keyPressed = _event->getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::W)
		{
			m_moveDirection.y = m_moveDirection.y == 0 ? -1 : 0;
		}
	}

	if (const auto* keyReleased = _event->getIf<sf::Event::KeyReleased>())
	{
		if (keyReleased->scancode == sf::Keyboard::Scancode::W)
		{
			m_moveDirection.y = 0;
		}
	}

	if (const auto* mouseButtonPressed = _event->getIf<sf::Event::MouseButtonPressed>())
	{
		if (mouseButtonPressed->button == sf::Mouse::Button::Left)
		{
			m_isShooting = true;
		}
	}

	if (const auto* mouseButtonReleased = _event->getIf<sf::Event::MouseButtonReleased>())
	{
		if (mouseButtonReleased->button == sf::Mouse::Button::Left)
		{
			m_isShooting = false;
		}
	}

	if (const auto* focusLost = _event->getIf<sf::Event::FocusLost>())
	{
		m_isNotFocused = true;

		m_moveDirection = sf::Vector2i(0, 0);
		m_isShooting = false;
	}

	if (const auto* focusGained = _event->getIf<sf::Event::FocusGained>())
	{
		m_isNotFocused = false;
	}
}

void Player::BotEvents(const float _dt)
{
	m_botChangeTimer -= _dt;
	m_shootCooldown -= _dt;

	if (m_botChangeTimer <= 0.0f)
	{
		std::uniform_real_distribution<float> posDist(0.0f, WORLD_SIZE);
		std::uniform_real_distribution<float> timeDist(0.3f, 1.0f);

		m_botTarget.x = posDist(m_rng);
		m_botTarget.y = posDist(m_rng);
		m_botChangeTimer = timeDist(m_rng);
	}

	m_lastWorldMousePosition = m_botTarget;

	sf::Vector2f shipPos = m_sprite->getPosition();
	sf::Vector2f toTarget = m_botTarget - shipPos;
	float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

	m_moveDirection.x = 0;

	const float stopDistance = 10.0f;

	if (distance > 1e-3f)
	{
		m_moveDirection.y = (distance > stopDistance) ? -1 : 0;

		float rotation = m_sprite->getRotation().asRadians();
		sf::Vector2f forwardDirection(std::cos(rotation), std::sin(rotation));
		sf::Vector2f dirNorm = (distance > 0.0001f) ? sf::Vector2f(toTarget.x / distance, toTarget.y / distance) : sf::Vector2f(0.0f, 0.0f);
		float forwardDot = dirNorm.x * forwardDirection.x + dirNorm.y * forwardDirection.y;

		const float aimThreshold = 0.10f;
		const float maxShootDistance = 600.0f;
		const float botShootCooldown = 0.10f;
		if (forwardDot >= aimThreshold && m_shootCooldown <= 0.0f)
		{
			m_isShooting = true;
			m_shootCooldown = botShootCooldown;
		}
		else
		{
			m_isShooting = false;
		}
	}
	else
	{
		m_moveDirection.y = 0;
		m_isShooting = false;
	}
}

void Player::Update(const float _dt, const sf::RenderWindow& _window)
{
	if (m_isBot)
	{
		BotEvents(_dt);
	}

	auto respawnMessage = client::Client::Get()->getTcpMessages("RespawnMessage");
	if (!respawnMessage.second.empty())
	{
		auto castedMessage = std::static_pointer_cast<engine::RespawnMessage>(respawnMessage.second.back());
		float positionX = castedMessage->m_positionX;
		float positionY = castedMessage->m_positionY;
		m_sprite->setPosition(sf::Vector2f(positionX, positionY));

		if (m_isBot && (m_botTarget.x == 0.0f && m_botTarget.y == 0.0f))
		{
			m_botTarget = m_sprite->getPosition();
		}
	}

	std::shared_ptr<engine::MovementInputMessage> movementInputMessage = std::make_shared<engine::MovementInputMessage>();

	UpdateRotation(movementInputMessage, _dt, _window);
	Move(movementInputMessage, _dt);

	client::Client::Get()->sendTcpMessage(movementInputMessage);

	if (m_isShooting)
	{
		std::shared_ptr<engine::ShootInputMessage> playerShootMessage = std::make_shared<engine::ShootInputMessage>();
		client::Client::Get()->sendTcpMessage(playerShootMessage);
	}
}

void Player::Draw(sf::RenderWindow& _window)
{
	_window.draw(*m_sprite);
}

void Player::UpdateRotation(std::shared_ptr<engine::MovementInputMessage> _movementInputMessage, const float _dt, const sf::RenderWindow& _window)
{
	if (m_isNotFocused)
	{
		_movementInputMessage->m_mouseWorldPosX = m_lastWorldMousePosition.x;
		_movementInputMessage->m_mouseWorldPosY = m_lastWorldMousePosition.y;
		return;
	}

	if (m_isBot)
	{
		_movementInputMessage->m_mouseWorldPosX = m_lastWorldMousePosition.x;
		_movementInputMessage->m_mouseWorldPosY = m_lastWorldMousePosition.y;

		sf::Vector2f worldMousePosition = m_lastWorldMousePosition;
		sf::Vector2f shipPosition = m_sprite->getPosition();

		float deltaX = worldMousePosition.x - shipPosition.x;
		float deltaY = worldMousePosition.y - shipPosition.y;
		float angleRadians = std::atan2(deltaY, deltaX) + 90.0f * (static_cast<float>(M_PI) / 180.0f);

		m_sprite->setRotation(sf::radians(angleRadians));
		return;
	}

	sf::Vector2i mousePosition = sf::Mouse::getPosition(_window);
	sf::Vector2f worldMousePosition = _window.mapPixelToCoords(mousePosition);
	m_lastWorldMousePosition = worldMousePosition;

	_movementInputMessage->m_mouseWorldPosX = worldMousePosition.x;
	_movementInputMessage->m_mouseWorldPosY = worldMousePosition.y;

	sf::Vector2f shipPosition = m_sprite->getPosition();

	float deltaX = worldMousePosition.x - shipPosition.x;
	float deltaY = worldMousePosition.y - shipPosition.y;
	float angleRadians = std::atan2(deltaY, deltaX) + 90.0f * (static_cast<float>(M_PI) / 180.0f);

	m_sprite->setRotation(sf::radians(angleRadians));
}

void Player::Move(std::shared_ptr<engine::MovementInputMessage> _movementInputMessage, const float _dt)
{
	_movementInputMessage->m_moveDirectionX = m_moveDirection.x;
	_movementInputMessage->m_moveDirectionY = m_moveDirection.y;

	float rotation = m_sprite->getRotation().asRadians();
	sf::Vector2f forwardDirection(std::cos(rotation), std::sin(rotation));
	sf::Vector2f rightDirection(-std::sin(rotation), std::cos(rotation));

	sf::Vector2f movement(0.0f, 0.0f);
	movement += forwardDirection * static_cast<float>(m_moveDirection.x) * MOVE_SPEED * _dt;
	movement += rightDirection * static_cast<float>(m_moveDirection.y) * MOVE_SPEED * _dt;

	m_sprite->move(movement);

	float shipToMouseDistance = std::sqrt((m_lastWorldMousePosition.x - m_sprite->getPosition().x) 
		* (m_lastWorldMousePosition.x - m_sprite->getPosition().x) 
		+ (m_lastWorldMousePosition.y - m_sprite->getPosition().y) 
		* (m_lastWorldMousePosition.y - m_sprite->getPosition().y));

	if (shipToMouseDistance < MOUSE_DEAD_ZONE)
	{
		m_sprite->move(-movement);
		return;
	}

	sf::Vector2f position = m_sprite->getPosition();
	if (position.x < 0.0f)
	{
		position.x += WORLD_SIZE;
	}	
	else if (position.x >= WORLD_SIZE)
	{
		position.x -= WORLD_SIZE;
	}

	if (position.y < 0.0f)
	{
		position.y += WORLD_SIZE;
	}	
	else if (position.y >= WORLD_SIZE)
	{
		position.y -= WORLD_SIZE;
	}

	m_sprite->setPosition(position);
}
