#ifndef TRA_CLIENT_CLIENT_HPP
#define TRA_CLIENT_CLIENT_HPP

#include "TRA/export.hpp"

#include <string>
#include <cstdint>

#include "TRA/errorCode.hpp"
#include "TRA/engine/networkEngine.hpp"

namespace tra::client
{
	using EntityId = uint32_t;

	class Client
	{
	public:
		Client(Client& other) = delete;
		void operator=(const Client&) = delete;

		TRA_API static Client* Get();

		TRA_API ErrorCode ConnectTo(const std::string& _address, uint16_t _port);
		TRA_API ErrorCode Disconnect();

		TRA_API bool IsConnected() const;

		TRA_API void beginUpdate();
		TRA_API void endUpdate();

		TRA_API ErrorCode sendTcpMessage(std::shared_ptr<engine::Message> _message);
		TRA_API std::pair<ErrorCode, std::vector<std::shared_ptr<engine::Message>>> getTcpMessages(const std::string& _messageType);

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
