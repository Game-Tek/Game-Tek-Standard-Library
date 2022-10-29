#pragma once

#include "IPEndpoints.h"
#include "GTSL/Range.hpp"

#if (_WIN32)
#include <WinSock2.h>
#include <intrin.h>
typedef int socklen_t;
#elif (__linux__)
#include <sys/socket.h>
#endif

namespace GTSL {
	inline uint16 HostToNet(const uint16 a) {
#ifdef _WIN32
		return _byteswap_ushort(a);
#else
		return (a >> 8) | (a << 8);
#endif
	}

	inline uint16 NetToHost(const uint16 a) {
#ifdef _WIN32
		return _byteswap_ushort(a);
#else
		return (a >> 8) | (a << 8);
#endif
	}

	class Socket {
#if (_WIN32)
		uint64 handle = 0;
#elif (__linux__)
        int handle = 0;
#endif

	public:
		Socket() = default;

		bool Open(const IPv4Endpoint endpoint, const bool blocking) {
#if (_WIN32)
			WSADATA wsa_data;
			if (const auto wsaStartResult = WSAStartup(MAKEWORD(2, 2), &wsa_data); wsaStartResult != 0) { return false; }

			handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(handle == INVALID_SOCKET) { return false; }

			sockaddr_in address{};
			address.sin_family = AF_INET;
			address.sin_addr.S_un.S_un_b.s_b1 = endpoint.Address[3];
			address.sin_addr.S_un.S_un_b.s_b2 = endpoint.Address[2];
			address.sin_addr.S_un.S_un_b.s_b3 = endpoint.Address[1];
			address.sin_addr.S_un.S_un_b.s_b4 = endpoint.Address[0];
			address.sin_port = HostToNet(endpoint.Port);
			if (bind(handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) < 0) { return false; }

			DWORD non_blocking = blocking;
			if (ioctlsocket(handle, FIONBIO, &non_blocking) != 0) { return false; }
#elif (__linux__)
            // open udp socket on linux
            handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(handle < 0) { return false; }

            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = *(reinterpret_cast<const uint32*>(endpoint.Address));
            address.sin_port = HostToNet(endpoint.Port);
            if (bind(handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) < 0) { return false; }

            // set non blocking
            int non_blocking = blocking;
            if (fcntl(handle, F_SETFL, O_NONBLOCK, non_blocking) == -1) { return false; }
#endif

			return true;
		}
		
		[[nodiscard]] bool Send(const IPv4Endpoint endpoint, Range<const byte*> buffer) const {
#if (_WIN32)
			sockaddr_in addr{};
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_un_b.s_b1 = endpoint.Address[3];
			addr.sin_addr.S_un.S_un_b.s_b2 = endpoint.Address[2];
			addr.sin_addr.S_un.S_un_b.s_b3 = endpoint.Address[1];
			addr.sin_addr.S_un.S_un_b.s_b4 = endpoint.Address[0];
			addr.sin_port = HostToNet(endpoint.Port);

			GTSL_ASSERT(buffer.Bytes() <= 16384, "Size bigger than can be sent.")
			const auto sentBytes = sendto(handle, reinterpret_cast<const char*>(buffer.begin()), static_cast<int32>(buffer.Bytes()), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));

			return sentBytes == static_cast<int32>(buffer.Bytes());
#elif (__linux__)
            // sendto on linux
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = *(reinterpret_cast<const uint32*>(endpoint.Address));
            addr.sin_port = HostToNet(endpoint.Port);

            GTSL_ASSERT(buffer.Bytes() <= 16384, "Size bigger than can be sent.")
            const auto sentBytes = sendto(handle, reinterpret_cast<const char*>(buffer.begin()), static_cast<int32>(buffer.Bytes()), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));

            return sentBytes == static_cast<int32>(buffer.Bytes());
#endif
		}

		[[nodiscard]] bool Receive(IPv4Endpoint* sender, Range<byte*> buffer) const {
#if (_WIN32)
			sockaddr_in from{};
			socklen_t fromLength{ sizeof(from) };

			const auto bytes_received = recvfrom(handle, reinterpret_cast<char*>(buffer.begin()), static_cast<int32>(buffer.Bytes()), 0, reinterpret_cast<sockaddr*>(&from), &fromLength);

			sender->Address[0] = from.sin_addr.S_un.S_un_b.s_b4;
			sender->Address[1] = from.sin_addr.S_un.S_un_b.s_b3;
			sender->Address[2] = from.sin_addr.S_un.S_un_b.s_b2;
			sender->Address[3] = from.sin_addr.S_un.S_un_b.s_b1;
			sender->Port = NetToHost(from.sin_port);

			return bytes_received != SOCKET_ERROR;
#elif (__linux__)
            // Receive on linux
            sockaddr_in from{};
            socklen_t fromLength{ sizeof(from) };

            const auto bytesReceived = recvfrom(handle, reinterpret_cast<char*>(buffer.begin()), static_cast<int32>(buffer.Bytes()), 0, reinterpret_cast<sockaddr*>(&from), &fromLength);

            sender->Address[0] = from.sin_addr.S_un.S_un_b.s_b4;
            sender->Address[1] = from.sin_addr.S_un.S_un_b.s_b3;
            sender->Address[2] = from.sin_addr.S_un.S_un_b.s_b2;
            sender->Address[3] = from.sin_addr.S_un.S_un_b.s_b1;
            sender->Port = NetToHost(from.sin_port);

            return bytesReceived > 0;
#endif
		}

		~Socket() {
#if (_WIN32)
			if (handle) {
				closesocket(handle);
				WSACleanup();
			}
#elif (__linux__)
            // Close socket on linux
            if (handle) {
                close(handle);
            }
#endif
		}
	};
}
