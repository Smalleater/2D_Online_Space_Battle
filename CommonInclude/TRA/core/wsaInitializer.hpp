#ifndef TRA_CORE_WSA_INITIALIZER_HPP
#define TRA_CORE_WSA_INITIALIZER_HPP

#ifdef _WIN32

#include "TRA/export.hpp"
#include "TRA/errorCode.hpp"

#include <WinSock2.h>

namespace tra::core
{
	class WSAInitializer
	{
	public:
		WSAInitializer(WSAInitializer& other) = delete;
		void operator=(const WSAInitializer&) = delete;

		TRA_API static WSAInitializer* Get();

		TRA_API ErrorCode Init();
		TRA_API void CleanUp();

	private:
		static WSAInitializer* m_singleton;

		WSADATA m_wsaData;

		WSAInitializer();
		~WSAInitializer();
	};
}

#endif

#endif
