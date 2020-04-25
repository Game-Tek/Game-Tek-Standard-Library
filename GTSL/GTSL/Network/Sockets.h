#pragma once

#include "Ipv4Endpoint.h"
#include "GTSL/Ranger.h"

namespace GTSL
{
	class UDPSocket
	{
		uint64 handle = 0;

	public:
		UDPSocket() = default;
		~UDPSocket() = default;

		struct CreateInfo
		{
			uint16 Port = 0;
			bool Blocking = false;
		};
		void Open(const CreateInfo& createInfo);
		void Close();
		
		struct SendInfo
		{
			IpEndpoint Endpoint;
			Ranger<byte> Buffer;
		};
		[[nodiscard]] bool Send(const SendInfo& sendInfo) const;


		struct ReceiveInfo
		{
			IpEndpoint* Sender = nullptr;
			Ranger<byte> Buffer;
		};
		[[nodiscard]] bool Receive(const ReceiveInfo& receiveInfo) const;
	};
}
