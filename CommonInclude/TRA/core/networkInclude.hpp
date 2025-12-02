#ifndef TRA_CORE_NETWORK_INCLUDE_HPP
#define TRA_CORE_NETWORK_INCLUDE_HPP

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

namespace tra::core
{
    using socket_t = SOCKET;

    #define CLOSE_SOCKET closesocket

    constexpr socket_t INVALID_SOCKET_FD = INVALID_SOCKET;
    constexpr int SHUTDOWN_BOTH = SD_BOTH;
    constexpr int SOCKET_NOT_CONNECTED = WSAENOTCONN;
    constexpr int SOCKET_CONNECTION_RESET = WSAECONNRESET;
}

#elif defined(__unix__) || defined(__APPLE__)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

namespace tra::core
{
    using socket_t = int;

    #define CLOSE_SOCKET close

    constexpr socket_t INVALID_SOCKET_FD = -1;
    constexpr int SHUTDOWN_BOTH = SHUT_RDWR;
    constexpr int SOCKET_NOT_CONNECTED = ENOTCONN;
    constexpr int SOCKET_CONNECTION_RESET = ECONNRESET;
}

#endif

#endif