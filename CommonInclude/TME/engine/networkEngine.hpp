#ifndef TME_ENGINE_NETWORK_ENGINE_HPP
#define TME_ENGINE_NETWORK_ENGINE_HPP

#include "TME/export.hpp"

#include <cstdint>

#include "TME/errorCode.hpp"
#include "TME/core/tcpSocket.hpp"
#include "TME/core/udpSocket.hpp"

#include "TME/engine/networkEcs.hpp"

namespace tme::engine
{
	struct Message;

	class NetworkEngine
	{
	public:
		TME_API NetworkEngine();
		TME_API ~NetworkEngine();

		TME_API ErrorCode startTcpListenOnPort(uint16_t _port, bool _blocking);
		TME_API ErrorCode startTcpConnectToAddress(const std::string& _address, uint16_t _port, bool _blocking);
		TME_API ErrorCode startUdpOnPort(uint16_t _port, bool _blocking);

		TME_API ErrorCode stopTcpListen();
		TME_API ErrorCode stopTcpConnect();
		TME_API ErrorCode stopUdp();

		TME_API void beginUpdate();
		TME_API void endUpdate();

		TME_API EntityId createEntity();
		TME_API void destroyEntity(EntityId _entityId);

		TME_API ErrorCode sendTcpMessage(EntityId _entityId, std::shared_ptr<Message> _message);
		TME_API std::pair<ErrorCode, std::vector<std::shared_ptr<Message>>> getTcpMessages(EntityId _entityId, const std::string& _messageType);

		TME_API EntityId getSelfEntityId();

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
