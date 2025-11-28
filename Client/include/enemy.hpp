#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <map>
#include "SFML/Graphics.hpp"

struct Enemy
{
	sf::Texture m_texture;
	sf::Sprite* m_sprite;
};

class EnemyManager
{
public:
	static void UpdateEnemis();
	static void DrawEnemis(sf::RenderWindow& window);
private:
	static std::map<int, Enemy> m_enemies;

	static void CheckNewEnemy();
	static void CheckDisconnectedEnemy();

	static void UpdateEnemyRotation();
};

#endif
