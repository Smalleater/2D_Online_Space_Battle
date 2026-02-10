#ifndef TRA_NETCODE_CORE_UDP_SOCKET_HPP
#define TRA_NETCODE_CORE_UDP_SOCKET_HPP

#include "TRA/export.hpp"
#include "TRA/errorCode.hpp"

#include <utility>
#include <mutex>
#include <string>

#include "TRA/netcode/core/networkInclude.hpp"

namespace tra::netcode::core
{
    class UdpSocket
    {
    public:
        TRA_API UdpSocket();
        TRA_API ~UdpSocket();

        TRA_API static sockaddr* createSockAddr(const std::string& _address, uint16_t _port);

        TRA_API void closeSocket();
        TRA_API std::pair<ErrorCode, int> bindSocket(const uint16_t _port);
        TRA_API std::pair<ErrorCode, int> sendDataTo(const void* _data, size_t _size, const sockaddr* _destAddr);
        TRA_API std::pair<ErrorCode, int> receiveDataFrom(void* _buffer, size_t _size, sockaddr* _srcAddr);
        TRA_API std::pair<ErrorCode, int> setBlocking(bool _blocking);
        TRA_API std::pair<ErrorCode, uint16_t> getPort();
        TRA_API bool isBlocking();
        TRA_API bool isOpen() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
        uint16_t m_port;
        bool m_isBlocking;
    };
}

#endif