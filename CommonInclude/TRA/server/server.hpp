#ifndef TRA_SERVER_SERVER_HPP
#define TRA_SERVER_SERVER_HPP

#include "TRA/export.hpp"

#include <cstdint>
#include <unordered_map>

#include "TRA/errorCode.hpp"
#include "TRA/engine/networkEngine.hpp"

namespace tra::engine
{
	struct Message;
}

namespace tra::server
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

		TRA_API ErrorCode sendTcpMessage(engine::EntityId _entityId, std::shared_ptr<engine::Message> _message);
		TRA_API std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> getTcpMessages(EntityId _entityId, const std::string& _messageType);

		TRA_API EntityId getSelfEntityId();

		template<typename ComponentType>
		bool entityHasComponent(EntityId _entityId)
		{
			return m_networkEngine->entityHasComponent<ComponentType>(_entityId);
		}

		template<typename ...ComponentType>
		std::vector<EntityId> queryEntityIds()
		{
			return m_networkEngine->queryEntityIds<ComponentType...>();
		}

		template<typename ...ComponentType>
		std::vector<std::tuple<EntityId, std::shared_ptr<ComponentType>...>> queryEntity()
		{
			return m_networkEngine->queryEntity<ComponentType...>();
		}

	private:
		static Server* m_singleton;

		engine::NetworkEngine* m_networkEngine;

		Server();
		~Server();
	};

	
}

#endif