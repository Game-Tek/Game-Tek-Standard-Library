#include "GTSL/Network/Sockets.h"

#include <iostream>

#include "GTSL/Assert.h"

using namespace GTSL;

#define WIN32_LEAN_AND_MEAN
#if (_WIN32)
#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

typedef int socklen_t;
#endif

void UDPSocket::Open(const CreateInfo& createInfo)
{
#if (_WIN32)
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	
	handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(createInfo.Port);
	if (bind(handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) < 0) { GTSL_ASSERT(true, "Failed to open socket!"); }

	DWORD nonBlocking = createInfo.Blocking;
	if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {}
#endif
}

void UDPSocket::Close()
{
#if (_WIN32)
	closesocket(handle);
	
	WSACleanup();
#endif
}

bool UDPSocket::Send(const SendInfo& sendInfo) const
{
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(sendInfo.Endpoint.IntFromAddress());
	addr.sin_port = htons(sendInfo.Endpoint.Port);

	const auto sent_bytes = sendto(handle, static_cast<const char*>(sendInfo.Buffer.Data()), sendInfo.Buffer.Bytes(), 0, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));

	return sent_bytes == static_cast<int32>(sendInfo.Buffer.Bytes());
}

bool UDPSocket::Receive(const ReceiveInfo& receiveInfo) const
{
	sockaddr_in from{};
	socklen_t fromLength{ sizeof(from) };

	const auto bytes_received = recvfrom(handle, reinterpret_cast<char*>(const_cast<unsigned char*>(receiveInfo.Buffer.begin())), receiveInfo.Buffer.Bytes(), 0, reinterpret_cast<sockaddr*>(&from), &fromLength);

	receiveInfo.Sender->AddressFromInt(ntohl(from.sin_addr.s_addr));
	receiveInfo.Sender->Port = ntohs(from.sin_port);

	return bytes_received != SOCKET_ERROR;
}
