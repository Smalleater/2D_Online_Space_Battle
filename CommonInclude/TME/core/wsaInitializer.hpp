#ifndef TME_CORE_WSA_INITIALIZER_HPP
#define TME_CORE_WSA_INITIALIZER_HPP

#ifdef _WIN32

#include "TME/export.hpp"
#include "TME/errorCode.hpp"

#include <WinSock2.h>

namespace tme::core
{
	class WSAInitializer
	{
	public:
		WSAInitializer(WSAInitializer& other) = delete;
		void operator=(const WSAInitializer&) = delete;

		TME_API static WSAInitializer* Get();

		TME_API ErrorCode Init();
		TME_API void CleanUp();

	private:
		static WSAInitializer* m_singleton;

		WSADATA m_wsaData;

		WSAInitializer();
		~WSAInitializer();
	};
}

#endif

#endif
