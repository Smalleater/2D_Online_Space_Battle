#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "TME/server/server.hpp"

struct Player
{
	tme::engine::EntityId m_id;
	float m_x = 0;
	float m_y = 0;
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

	static void updatePlayerRotation();
};

#endif
