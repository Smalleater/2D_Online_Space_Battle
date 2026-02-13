#ifndef TRA_NETCODE_SERVER_SERVER_HPP
#define TRA_NETCODE_SERVER_SERVER_HPP

#include "TRA/export.hpp"

#include "TRA/errorCode.hpp"

#include <cstdint>
#include <unordered_map>
#include <chrono>

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

		TRA_API ErrorCode start(uint16_t _port, uint8_t _tickRate = 20);
		TRA_API ErrorCode stop();

		TRA_API bool isRunning() const;

		TRA_API uint32_t getCurrentTick();
		TRA_API float getFixedDeltaTime();
		
		TRA_API bool canUpdateNetcode();

		TRA_API void updateElapsedTime();

		TRA_API void beginUpdate();
		TRA_API void endUpdate();

		TRA_API ecs::World* getEcsWorld();

		TRA_API ErrorCode sendTcpMessage(ecs::Entity _entity, std::shared_ptr<engine::Message> _message);
		TRA_API std::vector<std::shared_ptr<engine::Message>> getTcpMessages(ecs::Entity _entity, const std::string& _messageType);

	private:
		static Server* m_singleton;

		std::unique_ptr<engine::NetworkEngine> m_networkEngine;
		
		uint64_t m_currentTick;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_lastElapsedTimeUpdate;

		float m_fixedDeltaTime;
		float m_elapsedTime;

		uint8_t m_tickRate;
		std::chrono::high_resolution_clock m_clock;

		Server() = default;
		~Server();

		void initializeNewClient();
		void setClientReady();
	};
}

#endif