#ifndef TME_CORE_TCP_SOCKET_HPP
#define TME_CORE_TCP_SOCKET_HPP

#include "TME/export.hpp"

#include <utility>
#include <memory>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "TME/errorCode.hpp"
#include "networkInclude.hpp"

namespace tme::core
{
    class TcpSocket
    {
    public:
        TME_API TcpSocket();
        TME_API ~TcpSocket();

        TME_API std::pair<ErrorCode, int> shutdownSocket();
        TME_API void closeSocket();
        TME_API std::pair<ErrorCode, int> connectTo(const std::string& _adress, const uint16_t _port);
        TME_API std::pair<ErrorCode, int> bindSocket(const uint16_t _port);
        TME_API std::pair<ErrorCode, int> listenSocket(int _backlog = SOMAXCONN);
        TME_API std::pair<ErrorCode, int> acceptSocket(TcpSocket** _outClient);
        TME_API std::pair<ErrorCode, int> sendData(const void* _data, size_t _size, int& _byteSent);
        TME_API std::pair<ErrorCode, int> receiveData(std::vector<uint8_t>& _buffer);
        TME_API std::pair<ErrorCode, int> setBlocking(bool _blocking);
        TME_API std::pair<ErrorCode, uint16_t> getPort();
        TME_API bool isBlocking() const;
        TME_API bool isOpen() const;
        TME_API bool isConnected() const;

    private:
        socket_t m_socket;
        mutable std::mutex m_mutex;
		uint16_t m_port;
		bool m_isBlocking;
    };
}


#endif