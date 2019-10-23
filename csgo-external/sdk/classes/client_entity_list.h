#pragma once

#include <stdint.h>

#include "base_entity.h"


namespace sdk {
	class ClientEntityList {
	public:
		ClientEntityList() = default;
		explicit ClientEntityList(const uint32_t address) : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		// get an entity from their index
		BaseEntity get_entity(uint32_t index) const;

		// get localplayer entity 
		// all it does is call get_entity() with EngineClient::get_local_player_index()
		BaseEntity get_local_player() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk