#ifndef TRA_NETCODE_CORE_NET_UTILS_HPP
#define TRA_NETCODE_CORE_NET_UTILS_HPP

#include "TRA/export.hpp"

#include <string>
#include <cstdint>

namespace tra::netcode::core
{
	namespace NetUtils
	{
		TRA_API bool isValidIpV4Address(const std::string& _address);
		TRA_API bool isValidPort(uint16_t _port);
	}
}

#endif
