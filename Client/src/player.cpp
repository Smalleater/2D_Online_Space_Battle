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
		if (keyPressed->scancode == sf::Keyboard::Scancode::A)
		{
			m_moveDirection.x = m_moveDirection.x == 0 ? -1 : 0;
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::D)
		{
			m_moveDirection.x = m_moveDirection.x == 0 ? 1 : 0;
		}
	}

	if (const auto* keyReleased = _event->getIf<sf::Event::KeyReleased>())
	{
		if (keyReleased->scancode == sf::Keyboard::Scancode::W || keyReleased->scancode == sf::Keyboard::Scancode::S)
		{
			m_moveDirection.y = 0;
		}
		if (keyReleased->scancode == sf::Keyboard::Scancode::A || keyReleased->scancode == sf::Keyboard::Scancode::D)
		{
			m_moveDirection.x = 0;
		}
	}
}

void Player::Update(const float _dt, const sf::RenderWindow& _window)
{
	UpdateRotation(_dt, _window);
	Move(_dt);
}

void Player::Draw(sf::RenderWindow& _window)
{
	_window.draw(*m_sprite);
}

void Player::UpdateRotation(const float _dt, const sf::RenderWindow& _window)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(_window);
	sf::Vector2f worldMousePosition = _window.mapPixelToCoords(mousePosition);

	std::shared_ptr<engine::RotationInputMessage> rotationInputMessage = std::make_shared<engine::RotationInputMessage>();
	rotationInputMessage->m_mouseWorldPosX = worldMousePosition.x;
	rotationInputMessage->m_mouseWorldPosY = worldMousePosition.y;
	client::Client::Get()->sendTcpMessage(rotationInputMessage);

	sf::Vector2f shipPosition = m_sprite->getPosition();

	float deltaX = worldMousePosition.x - shipPosition.x;
	float deltaY = worldMousePosition.y - shipPosition.y;
	float angleRadians = std::atan2(deltaY, deltaX);
	float angleDegrees = angleRadians * 180.0f / static_cast<float>(M_PI) + 90.0f;

	m_sprite->setRotation(sf::degrees(angleDegrees));
}

void Player::Move(const float _dt)
{
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
