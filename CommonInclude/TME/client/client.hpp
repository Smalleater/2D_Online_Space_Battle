#ifndef TME_CLIENT_CLIENT_HPP
#define TME_CLIENT_CLIENT_HPP

#include "TME/export.hpp"

#include <string>
#include <cstdint>

#include "TME/errorCode.hpp"
#include "TME/engine/networkEngine.hpp"

namespace tme::client
{
	using EntityId = uint32_t;

	class Client
	{
	public:
		Client(Client& other) = delete;
		void operator=(const Client&) = delete;

		TME_API static Client* Get();

		TME_API ErrorCode ConnectTo(const std::string& _address, uint16_t _port);
		TME_API ErrorCode Disconnect();

		TME_API bool IsConnected() const;

		TME_API void beginUpdate();
		TME_API void endUpdate();

		TME_API ErrorCode sendTcpMessage(std::shared_ptr<engine::Message> _message);
		TME_API std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> getTcpMessages(const std::string& _messageType);

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
		static Client* m_singleton;

		engine::NetworkEngine* m_networkEngine;

		Client();
		~Client();
	};
}

#endif
