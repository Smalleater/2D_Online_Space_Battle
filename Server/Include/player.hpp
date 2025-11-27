#ifndef PLAYER_HPP

#include "TME/server/server.hpp"

using namespace tme;

struct Player
{
	engine::EntityId m_id;
	float m_x;
	float m_y;
};

#endif
