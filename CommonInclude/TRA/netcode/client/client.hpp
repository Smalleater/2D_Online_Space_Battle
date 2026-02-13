#ifndef TRA_NETCODE_CLIENT_CLIENT_HPP
#define TRA_NETCODE_CLIENT_CLIENT_HPP

#include "TRA/export.hpp"

#include "TRA/errorCode.hpp"

#include <string>
#include <cstdint>

#include "TRA/netcode/engine/networkEngine.hpp"

namespace tra::netcode::client
{
	using EntityId = uint32_t;

	class Client
	{
	public:
		Client(Client& other) = delete;
		void operator=(const Client&) = delete;

		TRA_API static Client* Get();

		TRA_API ErrorCode connectTo(const std::string& _address, uint16_t _port);
		TRA_API ErrorCode disconnect();

		TRA_API bool isConnected() const;
		TRA_API bool isReady() const;

		TRA_API uint32_t getCurrentTick();
		TRA_API float getFixedDeltaTime();

		TRA_API bool canUpdateNetcode();

		TRA_API void updateElapsedTime();

		TRA_API void beginUpdate();
		TRA_API void endUpdate();

		TRA_API ecs::World* getEcsWorld();

		TRA_API ErrorCode sendTcpMessage(std::shared_ptr<engine::Message> _message);
		TRA_API std::vector<std::shared_ptr<engine::Message>> getTcpMessages(const std::string& _messageType);

	private:
		static Client* m_singleton;

		std::unique_ptr<engine::NetworkEngine> m_networkEngine;

		Client() = default;
		~Client();

		void receiveInitializeClient();
	};
}

#endif
