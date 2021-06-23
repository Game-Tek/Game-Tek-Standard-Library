#pragma once

#include "IPEndpoints.h"
#include "GTSL/Range.h"

#if (_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <WinSock2.h>
#undef NOCOMM
#undef WIN32_LEAN_AND_MEAN
typedef int socklen_t;
#endif

namespace GTSL
{
	class UDPSocket
	{
		uint64 handle = 0;

	public:
		UDPSocket() = default;

		bool Open(const IPv4Endpoint endpoint, const bool blocking) {
#if (_WIN32)
			WSADATA wsa_data;
			if (const auto wsaStartResult = WSAStartup(MAKEWORD(2, 2), &wsa_data); wsaStartResult != 0) { return false; }

			handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			sockaddr_in address{};
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = htonl(endpoint.IntFromAddress());
			address.sin_port = htons(endpoint.Port);
			if (bind(handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) < 0) { return false; }

			DWORD non_blocking = blocking;
			if (ioctlsocket(handle, FIONBIO, &non_blocking) != 0) { return false; }
#endif

			return true;
		}
		
		[[nodiscard]] bool Send(const IPv4Endpoint endpoint, Range<const byte*> buffer) const {
			sockaddr_in addr{};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(endpoint.IntFromAddress());
			addr.sin_port = htons(endpoint.Port);

			const auto sent_bytes = sendto(handle, reinterpret_cast<const char*>(buffer.begin()), buffer.Bytes(), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));

			return sent_bytes == static_cast<int32>(buffer.Bytes());
		}

		[[nodiscard]] bool Receive(IPv4Endpoint* sender, Range<byte*> buffer) const {
			sockaddr_in from{};
			socklen_t fromLength{ sizeof(from) };

			const auto bytes_received = recvfrom(handle, reinterpret_cast<char*>(buffer.begin()), buffer.Bytes(), 0, reinterpret_cast<sockaddr*>(&from), &fromLength);

			sender->AddressFromInt(ntohl(from.sin_addr.s_addr));
			sender->Port = ntohs(from.sin_port);

			return bytes_received != SOCKET_ERROR;
		}

		~UDPSocket() {
#if (_WIN32)
			if (handle) {
				closesocket(handle);
				WSACleanup();
			}
#endif
		}
	};

	class TCPSocket
	{
		uint64 handle{ 0 };
	public:

		struct CreateInfo
		{
			bool Blocking = false; IPv4Endpoint Endpoint;
		};
		void Open(const CreateInfo& createInfo) {
#if (_WIN32)
			WSADATA wsa_data;
			WSAStartup(MAKEWORD(2, 2), &wsa_data);

			handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			sockaddr_in address{};
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = htonl(createInfo.Endpoint.IntFromAddress());
			address.sin_port = htons(createInfo.Endpoint.Port);
			if (connect(handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) < 0) { GTSL_ASSERT(true, "Failed to open socket!"); }

			DWORD non_blocking = createInfo.Blocking;
			if (ioctlsocket(handle, FIONBIO, &non_blocking) != 0) {}
#endif
		}
		
		~TCPSocket() {
			if (handle) {
				shutdown(handle, SD_SEND);
				closesocket(handle);
				WSACleanup();
			}
		}

		struct SendInfo
		{
			IPv4Endpoint Endpoint;
			Range<const byte*> Buffer;
		};
		[[nodiscard]] bool Send(const SendInfo& sendInfo) const {
			sockaddr_in addr{};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(sendInfo.Endpoint.IntFromAddress());
			addr.sin_port = htons(sendInfo.Endpoint.Port);

			const auto sent_bytes = sendto(handle, reinterpret_cast<const char*>(sendInfo.Buffer.begin()), sendInfo.Buffer.Bytes(), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));

			return sent_bytes == static_cast<int32>(sendInfo.Buffer.Bytes());
		}
		
		struct ReceiveInfo
		{
			IPv4Endpoint* Sender = nullptr;
			Range<byte*> Buffer;
		};
		[[nodiscard]] bool Receive(const ReceiveInfo& receiveInfo) const {
			sockaddr_in from{ 0 };
			socklen_t from_length{ sizeof(sockaddr_in) };

			const auto bytes_received = recvfrom(handle, reinterpret_cast<char*>(receiveInfo.Buffer.begin()), receiveInfo.Buffer.Bytes(), 0, reinterpret_cast<sockaddr*>(&from), &from_length);

			receiveInfo.Sender->AddressFromInt(ntohl(from.sin_addr.s_addr));
			receiveInfo.Sender->Port = ntohs(from.sin_port);

			return bytes_received != SOCKET_ERROR;
		}
	};
}
