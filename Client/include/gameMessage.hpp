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

DECLARE_MESSAGE_BEGIN(RotationInputMessage)
FIELD(float, m_mouseWorldPosX)
FIELD(float, m_mouseWorldPosY)
DECLARE_MESSAGE_END()

DECLARE_MESSAGE_BEGIN(UpdateRotationMessage)
FIELD(int, m_id)
FIELD(float, m_rotation)
DECLARE_MESSAGE_END()

#endif
