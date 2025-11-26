#ifndef TME_CORE_UDP_SOCKET_HPP
#define TME_CORE_UDP_SOCKET_HPP

#include "TME/export.hpp"

#include <utility>
#include <mutex>
#include <string>

#include "TME/errorCode.hpp"
#include "networkInclude.hpp"

namespace tme::core
{
    class UdpSocket
    {
    public:
        TME_API UdpSocket();
        TME_API ~UdpSocket();

        TME_API static sockaddr* createSockAddr(const std::string& _address, uint16_t _port);

        TME_API void closeSocket();
        TME_API std::pair<ErrorCode, int> bindSocket(const uint16_t _port);
        TME_API std::pair<ErrorCode, int> sendDataTo(const void* _data, size_t _size, const sockaddr* _destAddr);
        TME_API std::pair<ErrorCode, int> receiveDataFrom(void* _buffer, size_t _size, sockaddr* _srcAddr);
        TME_API std::pair<ErrorCode, int> setBlocking(bool _blocking);
        TME_API std::pair<ErrorCode, uint16_t> getPort();
        TME_API bool isBlocking();
        TME_API bool isOpen() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
        uint16_t m_port;
        bool m_isBlocking;
    };
}

#endif