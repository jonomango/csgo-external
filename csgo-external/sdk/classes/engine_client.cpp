#include "engine_client.h"

#include "../misc/constants.h"


namespace sdk {
	// get localplayer index
	int EngineClient::get_local_player() const {
		// ref @GetLocalPlayerIndex
		return globals::process.read<int>(globals::client_state + 
			offsets::client_state_local_player_index) + 1;
	}

	// check if player is in game
	bool EngineClient::is_in_game() const {
		// ref @IsInGame
		return globals::process.read<uint32_t>(globals::client_state + 
			offsets::client_state_signon_state) == 6; // SIGNONSTATE_FULL
	}

	// get the localplayer viewangles
	mango::Vec3f EngineClient::get_view_angles() const {
		// ref @GetViewAngles
		return globals::process.read<mango::Vec3f>(globals::client_state +
			offsets::client_state_view_angles);
	}

	// set the localplayer viewangles
	void EngineClient::set_view_angles(const mango::Vec3f& angles) const {
		// ref @GetViewAngles
		globals::process.write(globals::client_state + 
			offsets::client_state_view_angles, angles);
	}
} // namespace sdk