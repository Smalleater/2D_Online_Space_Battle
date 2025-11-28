#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "TME/server/server.hpp"

#include "vector.hpp"

struct Player
{
	tme::engine::EntityId m_id;
	Vector2f m_position;
	float m_rotation = 0;
};

class PlayerManager
{
public:
	static void removeDisconnectedPlayers();
	static void addNewPlayers();

	static void updatePlayers(float deltaTime);

private:
	static std::vector<Player> m_players;

	static void updatePlayerRotation(const float _mouseWorldPosX, const float _mouseWorldPosY, std::vector<Player>::iterator& _it);
	static void updatePlayerMovement(const int _moveDirectionX, const int _moveDirectionY, std::vector<Player>::iterator& _it, const float deltaTime);
};

#endif
