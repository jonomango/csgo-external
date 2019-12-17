#pragma once

#include "../common.h"

#include <stdint.h>
#include <epic/read_write_variable.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/cdll_client_int.cpp#L598
	class CHLClient {
	public:
		CHLClient() noexcept : m_address(0) {}
		explicit CHLClient(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		uint32_t chlclient() const noexcept { return this->m_address; }

	public:
		// get the head of the linked list of client classes
		mango::RWVariable<ClientClass> get_all_classes() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk