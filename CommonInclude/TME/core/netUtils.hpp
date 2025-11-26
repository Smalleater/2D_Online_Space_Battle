#ifndef TME_CORE_NET_UTILS_HPP
#define TME_CORE_NET_UTILS_HPP

#include "TME/export.hpp"

#include <string>
#include <cstdint>

namespace tme::core
{
	namespace NetUtils
	{
		TME_API bool isValidIpV4Address(const std::string& _address);
		TME_API bool isValidPort(uint16_t _port);
	}
}

#endif
