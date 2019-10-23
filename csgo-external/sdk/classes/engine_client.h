#pragma once

#include <stdint.h>

#include <misc/vector.h>


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/cdll_engine_int.cpp
	class EngineClient {
	public:
		EngineClient() = default;
		explicit EngineClient(const uint32_t address) : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		// get localplayer index
		uint32_t get_local_player() const;

		// check if player is in game
		bool is_in_game() const;

		// get the localplayer viewangles
		mango::vec3f get_view_angles() const;

		// set the localplayer viewangles
		void set_view_angles(const mango::vec3f& angles) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk