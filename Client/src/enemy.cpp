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
		m_enemies[id] = *enemy;
	}
}