#ifndef TRA_NETCODE_ENGINE_NETWORK_ENGINE_HPP
#define TRA_NETCODE_ENGINE_NETWORK_ENGINE_HPP

#include "TRA/export.hpp"
#include "TRA/errorCode.hpp"

#include <cstdint>

#include "TRA/ecs/world.hpp"

#include "TRA/netcode/core/tcpSocket.hpp"
#include "TRA/netcode/core/udpSocket.hpp"

#include "TRA/netcode/engine/message.hpp"

namespace tra::netcode::engine
{
	class NetworkEngine
	{
	public:
		TRA_API NetworkEngine();
		TRA_API ~NetworkEngine();

		TRA_API ErrorCode startTcpListenOnPort(uint16_t _port, bool _blocking);
		TRA_API ErrorCode startTcpConnectToAddress(const std::string& _address, uint16_t _port, bool _blocking);
		//TRA_API ErrorCode startUdpOnPort(uint16_t _port, bool _blocking);

		TRA_API void stopTcpListen();
		TRA_API void stopTcpConnect();
		//TRA_API ErrorCode stopUdp();

		TRA_API void beginUpdate();
		TRA_API void endUpdate();

		TRA_API ErrorCode sendTcpMessage(ecs::Entity _entity, std::shared_ptr<Message> _message);
		TRA_API std::vector<std::shared_ptr<Message>> getTcpMessages(ecs::Entity _entity, const std::string& _messageType);

		TRA_API ecs::World* getEcsWorld();
		TRA_API ecs::Entity getSelfEntity();

	private:
		//core::UdpSocket* m_udpSocket;

		std::unique_ptr<ecs::World> m_ecsWorld;

		ecs::Entity m_selfEntity;
	};
}

#endif
