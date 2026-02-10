#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <TRA/netcode/server/server.hpp>

#include "vector.hpp"

constexpr float PLAYER_RADIUS = 15.0f;

struct Player
{
	ecs::Entity m_entity;
	Vector2f m_position;
	Vector2f m_lastMousePosition;
	float m_rotation;
	float m_shootCooldown;
};

class PlayerManager
{
public:
	static void removeDisconnectedPlayers();
	static void addNewPlayers();

	static const std::vector<Player>& getPlayers() { return m_players; }

	static void updatePlayers(float deltaTime);
	static void playerHitByProjectile(const tra::ecs::Entity _playerEntity);

private:
	static std::vector<Player> m_players;
	static std::map<ecs::Entity, size_t> m_playersSparse;

	static Vector2f getRespawnPosition();

	static void updatePlayerRotation(const float _mouseWorldPosX, const float _mouseWorldPosY, std::vector<Player>::iterator& _it);
	static void updatePlayerMovement(const int _moveDirectionX, const int _moveDirectionY, std::vector<Player>::iterator& _it, const float deltaTime);
};

#endif
