#pragma once

#include "IPEndpoints.h"
#include "GTSL/Range.h"

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
			bool Blocking = false; IPv4Endpoint Endpoint;
		};
		void Open(const CreateInfo& createInfo);
		void Close();
		
		struct SendInfo
		{
			IPv4Endpoint Endpoint;
			Range<const byte*> Buffer;
		};
		[[nodiscard]] bool Send(const SendInfo& sendInfo) const;


		struct ReceiveInfo
		{
			IPv4Endpoint* Sender = nullptr;
			Range<byte*> Buffer;
		};
		[[nodiscard]] bool Receive(const ReceiveInfo& receiveInfo) const;
	};

	class TCPSocket
	{
		uint64 handle{ 0 };
	public:

		struct CreateInfo
		{
			bool Blocking = false; IPv4Endpoint Endpoint;
		};
		void Open(const CreateInfo& createInfo);
		void Close();

		struct SendInfo
		{
			IPv4Endpoint Endpoint;
			Range<const byte*> Buffer;
		};
		[[nodiscard]] bool Send(const SendInfo& sendInfo) const;


		struct ReceiveInfo
		{
			IPv4Endpoint* Sender = nullptr;
			Range<byte*> Buffer;
		};
		[[nodiscard]] bool Receive(const ReceiveInfo& receiveInfo) const;
	};
}
