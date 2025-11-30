#ifndef GAME_MESSAGE_HPP
#define GAME_MESSAGE_HPP

#include "TME/engine/message.hpp"

using namespace tme;

DECLARE_MESSAGE_BEGIN(NewClientMessage)
FIELD(int, m_id)
DECLARE_MESSAGE_END()

DECLARE_MESSAGE_BEGIN(DisconnectedClientMessage)
FIELD(int, m_id)
DECLARE_MESSAGE_END()

DECLARE_MESSAGE_BEGIN(MovementInputMessage)
FIELD(float, m_mouseWorldPosX)
FIELD(float, m_mouseWorldPosY)
FIELD(int, m_moveDirectionX)
FIELD(int, m_moveDirectionY)
DECLARE_MESSAGE_END()

DECLARE_MESSAGE_BEGIN(UpdateRotationAndPositionMessage)
FIELD(int, m_id)
FIELD(float, m_rotation)
FIELD(float, m_positionX)
FIELD(float, m_positionY)
DECLARE_MESSAGE_END()

DECLARE_MESSAGE_BEGIN(ShootInputMessage)
DECLARE_MESSAGE_END()


#endif
