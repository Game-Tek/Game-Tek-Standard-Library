#pragma once

#include "Ipv4Endpoint.h"

namespace GTSL
{
	class UDPSocket
	{
		uint64 handle = 0;

	public:
		struct CreateInfo
		{
			uint16 Port = 0;
			bool Blocking = false;
		};

		UDPSocket(const CreateInfo& createInfo);
		~UDPSocket();

		struct SendInfo
		{
			IpEndpoint Endpoint;
			void* Data = nullptr;
			uint32 Size = 0;
		};
		[[nodiscard]] bool Send(const SendInfo& sendInfo) const;


		struct ReceiveInfo
		{
			IpEndpoint* Sender = nullptr;
			void* Buffer = nullptr;
			uint32 BufferSize = 0;
		};
		[[nodiscard]] bool Receive(const ReceiveInfo& receiveInfo) const;
	};
}