#include "player.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "TME/client/client.hpp"
#include "gameMessage.hpp"

#define SPRITE_LOAD_PATH "resources/sprites/ship.png"

constexpr const float MoveSpeed = 200.0f;
constexpr const float WorldSize = 800.0f;

using namespace tme;

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
		if (keyPressed->scancode == sf::Keyboard::Scancode::S)
		{
			m_moveDirection.y = m_moveDirection.y == 0 ? 1 : 0;
		}
	}

	if (const auto* keyReleased = _event->getIf<sf::Event::KeyReleased>())
	{
		if (keyReleased->scancode == sf::Keyboard::Scancode::W || keyReleased->scancode == sf::Keyboard::Scancode::S)
		{
			m_moveDirection.y = 0;
		}
	}
}

void Player::Update(const float _dt, const sf::RenderWindow& _window)
{
	std::shared_ptr<engine::MovementInputMessage> movementInputMessage = std::make_shared<engine::MovementInputMessage>();

	UpdateRotation(movementInputMessage, _dt, _window);
	Move(movementInputMessage, _dt);

	client::Client::Get()->sendTcpMessage(movementInputMessage);
}

void Player::Draw(sf::RenderWindow& _window)
{
	_window.draw(*m_sprite);
}

void Player::UpdateRotation(std::shared_ptr<engine::MovementInputMessage> _movementInputMessage, const float _dt, const sf::RenderWindow& _window)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(_window);
	sf::Vector2f worldMousePosition = _window.mapPixelToCoords(mousePosition);

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
	movement += forwardDirection * static_cast<float>(m_moveDirection.x) * MoveSpeed * _dt;
	movement += rightDirection * static_cast<float>(m_moveDirection.y) * MoveSpeed * _dt;

	m_sprite->move(movement);

	sf::Vector2f position = m_sprite->getPosition();
	if (position.x < 0.0f)
	{
		position.x += WorldSize;
	}	
	else if (position.x >= WorldSize)
	{
		position.x -= WorldSize;
	}

	if (position.y < 0.0f)
	{
		position.y += WorldSize;
	}	
	else if (position.y >= WorldSize)
	{
		position.y -= WorldSize;
	}

	m_sprite->setPosition(position);
}
