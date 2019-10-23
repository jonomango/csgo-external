#pragma once

#include <stdint.h>


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/client/cdll_client_int.cpp
	class Client {
	public:
		Client() = default;
		explicit Client(const uint32_t address) : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:

		// get the head of the linked list of client classes
		uint32_t get_all_classes() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk