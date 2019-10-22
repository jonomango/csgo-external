#include "engine_client.h"

#include "../constants.h"


namespace sdk {
	// get localplayer index
	uint32_t EngineClient::get_local_player_index() const {
		// reversed from @GetLocalPlayerIndex
		return globals::process.read<uint32_t>(globals::client_state + 
			offsets::client_state_local_player_index) + 1;
	}
} // namespace sdk