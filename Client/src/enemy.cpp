#include "enemy.hpp"

#include "TME/client/client.hpp"
#include "TME/engine/message.hpp"
#include "gameMessage.hpp"

#define SPRITE_LOAD_PATH "resources/sprites/enemy.png"

using namespace tme;

std::map<int, Enemy> EnemyManager::m_enemies;

void EnemyManager::UpdateEnemis()
{
	CheckNewEnemy();
	CheckDisconnectedEnemy();

	UpdateEnemyRotation();
}

void EnemyManager::DrawEnemis(sf::RenderWindow& window)
{
	for (auto& [id, enemy] : m_enemies)
	{
		window.draw(*enemy.m_sprite);
	}
}

void EnemyManager::CheckNewEnemy()
{
	auto newEnemys = client::Client::Get()->getTcpMessages("NewClientMessage");
	for (auto newEnemy : newEnemys.second)
	{
		auto castedMessage = std::static_pointer_cast<engine::NewClientMessage>(newEnemy);
		int id = castedMessage->m_id;

		Enemy* enemy = new Enemy();
		if (!enemy->m_texture.loadFromFile(SPRITE_LOAD_PATH))
		{
			throw std::runtime_error("Failed to load player sprite texture from " SPRITE_LOAD_PATH);
		}
		enemy->m_sprite = new sf::Sprite(enemy->m_texture);
		enemy->m_sprite->setOrigin(sf::Vector2f(enemy->m_texture.getSize().x / 2.f, enemy->m_texture.getSize().y / 2.f));
		m_enemies[id] = *enemy;
	}
}

void EnemyManager::CheckDisconnectedEnemy()
{
	auto disconnectedEnemys = client::Client::Get()->getTcpMessages("DisconnectedClientMessage");
	for (auto disconnectedEnemy : disconnectedEnemys.second)
	{
		auto castedMessage = std::static_pointer_cast<engine::DisconnectedClientMessage>(disconnectedEnemy);
		int id = castedMessage->m_id;
		m_enemies.erase(id);
	}
}

void EnemyManager::UpdateEnemyRotation()
{
	auto rotationUpdates = client::Client::Get()->getTcpMessages("UpdateRotationMessage");
	for (auto rotationUpdate : rotationUpdates.second)
	{
		auto castedMessage = std::static_pointer_cast<engine::UpdateRotationMessage>(rotationUpdate);
		int id = castedMessage->m_id;
		float rotation = castedMessage->m_rotation;
		auto it = m_enemies.find(id);
		if (it != m_enemies.end())
		{
			it->second.m_sprite->setRotation(sf::degrees(rotation));
		}
	}
}