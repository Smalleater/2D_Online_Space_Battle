#ifndef TRA_NETCODE_CORE_TCP_SOCKET_HPP
#define TRA_NETCODE_CORE_TCP_SOCKET_HPP

#include "TRA/export.hpp"
#include "TRA/errorCode.hpp"

#include <utility>
#include <memory>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "TRA/netcode/core/networkInclude.hpp"

namespace tra::netcode::core
{
    class TcpSocket
    {
    public:
        TRA_API TcpSocket();
        TRA_API ~TcpSocket();

        TRA_API std::pair<ErrorCode, int> shutdownSocket();
        TRA_API void closeSocket();
        TRA_API std::pair<ErrorCode, int> connectTo(const std::string& _adress, const uint16_t _port);
        TRA_API std::pair<ErrorCode, int> bindSocket(const uint16_t _port);
        TRA_API std::pair<ErrorCode, int> listenSocket(int _backlog = SOMAXCONN);
        TRA_API std::pair<ErrorCode, int> acceptSocket(TcpSocket** _outClient);
        TRA_API std::pair<ErrorCode, int> sendData(const void* _data, size_t _size, int& _byteSent);
        TRA_API std::pair<ErrorCode, int> receiveData(std::vector<uint8_t>& _buffer);
        TRA_API std::pair<ErrorCode, int> setBlocking(bool _blocking);
        TRA_API std::pair<ErrorCode, uint16_t> getPort();
        TRA_API bool isBlocking() const;
        TRA_API bool isOpen() const;
        TRA_API bool isConnected() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
		uint16_t m_port;
		bool m_isBlocking;
    };
}


#endif