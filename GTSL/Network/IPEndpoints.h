#pragma once

#include "GTSL/Core.h"

namespace GTSL
{
	/**
	 * \brief Specifies and IP endpoint. This is an address and a port.
	 * Used for socket connections.
	 */
	struct IPv4Endpoint
	{
		uint8 Address[4]{ 0 };
		uint16 Port = 0;

		IPv4Endpoint() = default;
		constexpr IPv4Endpoint(const uint8 a, const uint8 b, const uint8 c, const uint8 d, const uint8 p) : Address{ a, b, c, d }, Port(p) {}
		
		/**
		 * \brief Returns an element of the IP address from an index.
		 * \param index uint8 referring to which of the 4 bytes in the IP address to retrieve.
		 * \return uint8 containing the solicited element of the address.
		 */
		uint8 operator[](const uint8 index) { return Address[index]; }

		bool operator!=(const IPv4Endpoint other) const {
			return Address[0] != other.Address[0] || Address[1] != other.Address[1] || Address[2] != other.Address[2] || Address[3] != other.Address[3] || Port != other.Port;
		}
		
		/**
		 * \brief Returns an int packed with the 4 byte values of the IP address this IpEndpoint holds.
		 * \return uint32 packed with the 4 byte values of the IP address this IpEndpoint holds.
		 */
		[[nodiscard]] uint32 GetAddress() const {
			return (Address[0] << 24) | (Address[1] << 16) | (Address[2] << 8) | Address[3];
		}

		/**
		 * \brief Sets this IpEndpoint's address as the passed in int_address uint32.
		 * \param intAddress IP address packed in uint32 from which to build the address.
		 */
		void SetAddress(const uint32 intAddress) {
			Address[0] = ((intAddress >> 24) & 0xFF); Address[1] = ((intAddress >> 16) & 0xFF); Address[2] = ((intAddress >> 8) & 0xFF); Address[3] = (intAddress & 0xFF);
		}
	};
}