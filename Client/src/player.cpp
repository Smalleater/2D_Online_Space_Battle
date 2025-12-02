#include "player.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "TRA/client/client.hpp"
#include "gameMessage.hpp"

#define SPRITE_LOAD_PATH "resources/sprites/ship.png"

constexpr float MOVE_SPEED = 200.0f;
constexpr float WORLD_SIZE = 800.0f;
constexpr float MOUSE_DEAD_ZONE = 5.0f;

using namespace tra;

Player::Player()
{
	if (!m_spriteTexture.loadFromFile(SPRITE_LOAD_PATH))
	{
		throw std::runtime_error("Failed to load player sprite texture from " SPRITE_LOAD_PATH);
	}

	m_sprite = new sf::Sprite(m_spriteTexture);
	m_sprite->setOrigin(sf::Vector2f(m_spriteTexture.getSize().x / 2.0f, m_spriteTexture.getSize().y / 2.0f));
}

Player::~Player()
{

}

void Player::PollEvents(const std::optional<sf::Event>& _event, const float _dt)
{
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
}

void Player::Update(const float _dt, const sf::RenderWindow& _window)
{
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
	sf::Vector2i mousePosition = sf::Mouse::getPosition(_window);
	sf::Vector2f worldMousePosition = _window.mapPixelToCoords(mousePosition);
	m_lastWorldMousePosition = worldMousePosition;

	_movementInputMessage->m_mouseWorldPosX = worldMousePosition.x;
	_movementInputMessage->m_mouseWorldPosY = worldMousePosition.y;

	sf::Vector2f shipPosition = m_sprite->getPosition();

	float deltaX = worldMousePosition.x - shipPosition.x;
	float deltaY = worldMousePosition.y - shipPosition.y;
	float angleRadians = std::atan2(deltaY, deltaX) + +90.0f * (static_cast<float>(M_PI) / 180.0f);

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
