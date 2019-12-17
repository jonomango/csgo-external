#pragma once

#include <stdint.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/iclientmode.h#L34
	class IClientMode {
	public:
		IClientMode() = default;
		explicit IClientMode(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		uint32_t iclientmode() const noexcept { return this->m_address; }

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk