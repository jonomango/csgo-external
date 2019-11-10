#pragma once

#include <stdint.h>

#include <misc/vector.h>

#include "../misc/interface_base.h"


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/cdll_engine_int.cpp
	class EngineClient : public InterfaceBase<EngineClient,
		mango::fnv1a<uint64_t>("engine.dll:VEngineClient")> {
	public:
		// get localplayer index
		int get_local_player() const;

		// check if player is in game
		bool is_in_game() const;

		// get the localplayer viewangles
		mango::Vec3f get_view_angles() const;

		// set the localplayer viewangles
		void set_view_angles(const mango::Vec3f& angles) const;
	};
} // namespace sdk