#ifndef GAME_MESSAGE_HPP

#include "TME/engine/message.hpp"

using namespace tme;

DECLARE_MESSAGE_BEGIN(NewClientMessage)
FIELD(int, m_id)
DECLARE_MESSAGE_END()

#endif
