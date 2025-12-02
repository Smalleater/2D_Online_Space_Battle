#ifndef TRA_ENGINE_NETWORK_ENGINE_HPP
#define TRA_ENGINE_NETWORK_ENGINE_HPP

#include "TRA/export.hpp"

#include <cstdint>

#include "TRA/errorCode.hpp"
#include "TRA/core/tcpSocket.hpp"
#include "TRA/core/udpSocket.hpp"

#include "TRA/engine/networkEcs.hpp"

namespace tra::engine
{
	struct Message;

	class NetworkEngine
	{
	public:
		TRA_API NetworkEngine();
		TRA_API ~NetworkEngine();

		TRA_API ErrorCode startTcpListenOnPort(uint16_t _port, bool _blocking);
		TRA_API ErrorCode startTcpConnectToAddress(const std::string& _address, uint16_t _port, bool _blocking);
		TRA_API ErrorCode startUdpOnPort(uint16_t _port, bool _blocking);

		TRA_API ErrorCode stopTcpListen();
		TRA_API ErrorCode stopTcpConnect();
		TRA_API ErrorCode stopUdp();

		TRA_API void beginUpdate();
		TRA_API void endUpdate();

		TRA_API EntityId createEntity();
		TRA_API void destroyEntity(EntityId _entityId);

		TRA_API ErrorCode sendTcpMessage(EntityId _entityId, std::shared_ptr<Message> _message);
		TRA_API std::pair<ErrorCode, std::vector<std::shared_ptr<Message>>> getTcpMessages(EntityId _entityId, const std::string& _messageType);

		TRA_API EntityId getSelfEntityId();

		template<typename ComponentType>
		ErrorCode addComponentToEntity(EntityId _entityId, std::shared_ptr<ComponentType> _component)
		{
			return m_networkEcs->addComponentToEntity(_entityId, _component);
		}

		template<typename ComponentType>
		bool entityHasComponent(EntityId _entityId)
		{
			return m_networkEcs->hasComponent<ComponentType>(_entityId);
		}

		template<typename ...ComponentType>
		std::vector<EntityId> queryEntityIds()
		{
			return m_networkEcs->queryIds<ComponentType...>();
		}

		template<typename ...ComponentType>
		std::vector<std::tuple<EntityId, std::shared_ptr<ComponentType>...>> queryEntity()
		{
			return m_networkEcs->query<ComponentType...>();
		}

	private:
		core::UdpSocket* m_udpSocket;

		NetworkEcs* m_networkEcs;

		EntityId m_selfEntityId;
	};
}

#endif
