#ifndef TRA_NETCODE_SERVER_SERVER_HPP
#define TRA_NETCODE_SERVER_SERVER_HPP

#include "TRA/export.hpp"

#include "TRA/errorCode.hpp"

#include <cstdint>
#include <unordered_map>

#include "TRA/netcode/engine/networkEngine.hpp"

namespace tra::netcode::server
{
	using EntityId = uint32_t;

	class Server
	{
	public:
		Server(Server& other) = delete;
		void operator=(const Server&) = delete;

		TRA_API static Server* Get();

		TRA_API ErrorCode Start(uint16_t _port);
		TRA_API ErrorCode Stop();

		TRA_API bool isRunning() const;

		TRA_API void beginUpdate();
		TRA_API void endUpdate();

		TRA_API ecs::World* getEcsWorld();

		TRA_API ErrorCode sendTcpMessage(ecs::Entity _entity, std::shared_ptr<engine::Message> _message);
		TRA_API std::vector<std::shared_ptr<engine::Message>> getTcpMessages(ecs::Entity _entity, const std::string& _messageType);

	private:
		static Server* m_singleton;

		engine::NetworkEngine* m_networkEngine;

		Server();
		~Server();
	};
}

#endif