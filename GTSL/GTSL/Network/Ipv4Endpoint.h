#pragma once

#include "../Core.h"

namespace GTSL
{
	/**
	 * \brief Specifies and IP endpoint. This is an address and a port.
	 * Used for socket connections.
	 */
	struct IpEndpoint
	{
		uint8 Address[4]{};
		uint16 Port = 0;

		/**
		 * \brief Returns an element of the IP address from an index.
		 * \param index uint8 referring to which of the 4 bytes in the IP address to retrieve.
		 * \return uint8 containing the solicited element of the address.
		 */
		uint8 operator[](const uint8 index) { return Address[index]; }

		/**
		 * \brief Returns an int packed with the 4 byte values of the IP address this IpEndpoint holds.
		 * \return uint32 packed with the 4 byte values of the IP address this IpEndpoint holds.
		 */
		[[nodiscard]] uint32 IntFromAddress() const
		{
			return (Address[0] << 24) | (Address[1] << 16) | (Address[2] << 8) | Address[3];
		}

		/**
		 * \brief Sets this IpEndpoint's address as the passed in int_address uint32.
		 * \param int_address IP address packed in uint32 from which to build the address.
		 */
		void AddressFromInt(const uint32 int_address)
		{
			Address[0] = ((int_address >> 24) & 0xFF), Address[1] = ((int_address >> 16) & 0xFF), Address[2] = ((int_address >> 8) & 0xFF), Address[3] = (int_address & 0xFF);
		}
	};
}