#ifndef TME_SERVER_SERVER_HPP
#define TME_SERVER_SERVER_HPP

#include "TME/export.hpp"

#include <cstdint>
#include <unordered_map>

#include "TME/errorCode.hpp"
#include "TME/engine/networkEngine.hpp"

namespace tme::engine
{
	struct Message;
}

namespace tme::server
{
	using EntityId = uint32_t;

	class Server
	{
	public:
		Server(Server& other) = delete;
		void operator=(const Server&) = delete;

		TME_API static Server* Get();

		TME_API ErrorCode Start(uint16_t _port);
		TME_API ErrorCode Stop();

		TME_API bool isRunning() const;

		TME_API void beginUpdate();
		TME_API void endUpdate();

		TME_API ErrorCode sendTcpMessage(engine::EntityId _entityId, std::shared_ptr<engine::Message> _message);
		TME_API std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> getTcpMessages(EntityId _entityId, const std::string& _messageType);

		TME_API EntityId getSelfEntityId();

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