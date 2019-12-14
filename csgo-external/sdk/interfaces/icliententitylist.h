#pragma once

#include "../classes/c_csplayer.h"

#include <stdint.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icliententitylist.h#L28
	class IClientEntityList {
	public:
		IClientEntityList() noexcept : m_address(0) {}
		explicit IClientEntityList(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		constexpr operator uint32_t() const noexcept { return this->m_address; }

	public:
		// get an entity by their index
		template <typename T = IClientEntity>
		T get_client_entity(int index) const { return T(this->get_client_entity_imp(index)); }

		// get localplayer entity 
		// all this does is call get_client_entity() with EngineClient::get_local_player_index()
		C_CSPlayer get_local_player() const;

	private:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icliententitylist.h#L38
		uint32_t get_client_entity_imp(int index) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk