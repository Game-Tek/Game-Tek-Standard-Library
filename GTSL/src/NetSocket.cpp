#include "Network/NetSocket.h"
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#if (_WIN32)
#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

typedef int socklen_t;

class WSAStart
{
	
	WSAStart()
	{
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(2, 2), &wsa_data);
	}
	
	~WSAStart()
	{
		WSACleanup();
	}
	
static WSAStart start;
};

WSAStart WSAStart::start;
#endif

NetSocket::NetSocket(const CreateInfo& createInfo)
{
#if (_WIN32)
	handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(createInfo.Port);
	if (bind(handle, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) < 0) { __debugbreak(); }

	DWORD nonBlocking = createInfo.Blocking;
	if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {}
#endif
}

NetSocket::~NetSocket()
{
#if (_WIN32)
	closesocket(handle);
#endif
}

bool NetSocket::Send(const SendInfo& sendInfo) const
{
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(sendInfo.Endpoint.IntFromAddress());
	addr.sin_port = htons(sendInfo.Endpoint.Port);

	const auto sent_bytes = sendto(handle, static_cast<const char*>(sendInfo.Data), sendInfo.Size, 0, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));

	return sent_bytes == sendInfo.Size;
}

bool NetSocket::Receive(const ReceiveInfo& receiveInfo) const
{
	sockaddr_in from{};
	socklen_t fromLength = sizeof(from);

	const auto bytes_received = recvfrom(handle, reinterpret_cast<char*>(receiveInfo.Buffer), receiveInfo.BufferSize, 0, reinterpret_cast<sockaddr*>(&from), &fromLength);

	receiveInfo.Sender->AddressFromInt(ntohl(from.sin_addr.s_addr));
	receiveInfo.Sender->Port = ntohs(from.sin_port);

	return bytes_received != SOCKET_ERROR;
}
