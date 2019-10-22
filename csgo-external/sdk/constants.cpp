#include "constants.h"

#include "interfaces.h"
#include "classes/client_entity_list.h"


namespace sdk {
	void setup_constants() {
		// load interfaces
		InterfaceCache interface_cache(globals::process);

		// get interfaces
		globals::client = Client(interface_cache.get_interface("client_panorama.dll", "VClient"));
		globals::engine_client = EngineClient(interface_cache.get_interface("engine.dll", "VEngineClient"));
		globals::client_entity_list = ClientEntityList(interface_cache.get_interface("client_panorama.dll", "VClientEntityList"));

		// GetLocalPlayerIndex vfunc
		const auto get_local_player_index = globals::process.get_vfunc<uint32_t>(
			globals::engine_client, indices::get_local_player_index);

		// get client state address from the mov operand
		const auto client_state_addr = globals::process.read<uint32_t>(get_local_player_index + 0x10);
		globals::client_state = globals::process.read<uint32_t>(client_state_addr);

		// dynamically get the local player index (reversed from @GetLocalPlayerIndex)
		offsets::client_state_local_player_index = 
			globals::process.read<uint32_t>(get_local_player_index + 0x16);
	}
} // namespace sdk