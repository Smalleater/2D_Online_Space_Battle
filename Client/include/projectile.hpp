#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>

struct Projectile
{
	Projectile(sf::Sprite _sprite) : m_sprite(_sprite) {};
	~Projectile() = default;
	sf::Sprite m_sprite;
};

class ProjectileManager
{
public:
	static void UpdateProjectiles(const float deltaTime);
	static void DrawProjectiles(sf::RenderWindow& window);

private:
	static sf::Texture* m_texture;
	static std::vector<Projectile> m_projectiles;

	static void CreateProjectile(const sf::Vector2f& position, const sf::Vector2f& direction);
};

#endif
