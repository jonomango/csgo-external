#pragma once

#include <stdint.h>


namespace sdk {
	class Client {
	public:
		Client() = default;
		explicit Client(const uint32_t address) : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	private:
		uint32_t m_address;
	};
} // namespace sdk