#include "projectile.hpp"

#include <TRA/client/client.hpp>

#include "gameMessage.hpp"

#define SPRITE_LOAD_PATH "resources/sprites/projectile.png"


sf::Texture* ProjectileManager::m_texture = nullptr;
std::vector<std::pair<int, Projectile>> ProjectileManager::m_projectiles;

using namespace tra;

void ProjectileManager::UpdateProjectiles(const float deltaTime)
{
	auto newProjectileMessages = client::Client::Get()->getTcpMessages("NewProjectileMessage");
	for (auto newProjectileMessage : newProjectileMessages.second)
	{
		auto castedMessage = std::static_pointer_cast<engine::NewProjectileMessage>(newProjectileMessage);
		float positionX = castedMessage->m_positionX;
		float positionY = castedMessage->m_positionY;
		float directionX = castedMessage->m_directionX;
		float directionY = castedMessage->m_directionY;

		CreateProjectile(castedMessage->m_projectileId, sf::Vector2f(positionX, positionY), sf::Vector2f(directionX, directionY));
	}

	auto updateProjectilePositionMessages = client::Client::Get()->getTcpMessages("UpdateProjectilePositionMessage");
	for (auto updateProjectilePositionMessage : updateProjectilePositionMessages.second)
	{
		auto castedMessage = std::static_pointer_cast<engine::UpdateProjectilePositionMessage>(updateProjectilePositionMessage);
		int projectileId = castedMessage->m_projectileId;
		float positionX = castedMessage->m_positionX;
		float positionY = castedMessage->m_positionY;

		for (auto& [id, projectile] : m_projectiles)
		{
			if (id == projectileId)
			{
				projectile.m_sprite.setPosition(sf::Vector2f(positionX, positionY));
				break;
			}
		}
	}
}

void ProjectileManager::DrawProjectiles(sf::RenderWindow& window)
{
	for (auto& [id, projectile] : m_projectiles)
	{
		window.draw(projectile.m_sprite);
	}
}

void ProjectileManager::CreateProjectile(const uint32_t _id, const sf::Vector2f& position, const sf::Vector2f& direction)
{
	if (m_texture == nullptr)
	{
		m_texture = new sf::Texture();
		if (!m_texture->loadFromFile(SPRITE_LOAD_PATH))
		{
			throw std::runtime_error("Failed to load projectile sprite texture from " SPRITE_LOAD_PATH);
		}
	}

	sf::Sprite sprite(*m_texture);
	sprite.setOrigin(sf::Vector2f(m_texture->getSize().x / 2.f, m_texture->getSize().y / 2.f));
	sprite.setPosition(position);

	float angle = std::atan2(direction.y, direction.x);
	sprite.setRotation(sf::radians(angle));

	m_projectiles.emplace_back(_id, Projectile(sprite));
}
