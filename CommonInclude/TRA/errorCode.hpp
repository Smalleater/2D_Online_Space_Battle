#ifndef TRA_API_ERRORCODE_HPP
#define TRA_API_ERRORCODE_HPP

namespace tra
{
	enum class ErrorCode
	{
		Success = 0,
		Failure,

		// Client/Server Error
		DisconnectWithErrors,

		// Server Error
		ServerAlreadyStarted,
		ServerNotRunning,

		// Client Error
		ClientAlreadyConnected,
		ClientNotConnected,

		// Engine Error
		NetworkEngineNotInitialized,
		InvalidIpAddress,
		InvalidPortNumber,
		InvalidComponent,

		// Network ECS Error
		EntityDoesNotHaveComponent,
		EntityAlreadyHasComponent,
		ComponentStoreUnavailable,
		InvalidEntity,
		EntityDoesNotExist,

		// Socket Error
		SocketAlreadyOpen,
		SocketNotOpen,
		SocketConnectionClosed,
		SocketWouldBlock,
		SocketShutdownFailed,
		SocketGetaddrinfoFailed,
		SocketConnectFailed,
		SocketBindFailed,
		SocketListenFailed,
		SocketAcceptFailed,
		SocketSendSizeTooLarge,
		SocketSendFailed,
		SocketSendPartial,
		SocketReceiveSizeTooLarge,
		SocketReceiveFailed,
		SocketSetBlockingFailed,
		SocketGetPortFailed,

		// WSA Error
		WSAStartupFailed
	};
}

#endif