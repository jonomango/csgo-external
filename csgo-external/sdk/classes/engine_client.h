#pragma once

#include <stdint.h>


namespace sdk {
	class EngineClient {
	public:
		EngineClient() = default;
		explicit EngineClient(const uint32_t address) : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

		// get localplayer index
		uint32_t get_local_player_index() const;

	private:
		uint32_t m_address;
	};
} // namespace sdk