#include "constants.h"

#include "interface_cache.h"
#include "../classes/client_entity_list.h"


namespace sdk {
	void setup_constants() {
		// load interfaces
		InterfaceCache interface_cache(globals::process);

		// get interfaces
		globals::client = Client(interface_cache.get("client_panorama.dll", "VClient"));
		globals::engine_client = EngineClient(interface_cache.get("engine.dll", "VEngineClient"));
		globals::client_entity_list = ClientEntityList(interface_cache.get("client_panorama.dll", "VClientEntityList"));

		// @GetLocalPlayerIndex
		const auto get_local_player_index = globals::process.get_vfunc<uint32_t>(
			globals::engine_client, indices::get_local_player_index);

		// get client state address from the mov operand
		const auto client_state_addr = globals::process.read<uint32_t>(get_local_player_index + 0x10);
		globals::client_state = globals::process.read<uint32_t>(client_state_addr);

		// dynamically get the local player index offset (@GetLocalPlayerIndex)
		offsets::client_state_local_player_index = 
			globals::process.read<uint32_t>(get_local_player_index + 0x16);

		// dynamically get the signon state offset (@IsInGame)
		const auto is_in_game = globals::process.get_vfunc<uint32_t>(
			globals::engine_client, indices::is_in_game);
		offsets::client_state_signon_state = globals::process.read<uint32_t>(is_in_game + 0x7);

		// dynamically get the viewangles offset (@GetViewAngles)
		const auto get_view_angles = globals::process.get_vfunc<uint32_t>(
			globals::engine_client, indices::get_view_angles);
		offsets::client_state_view_angles = globals::process.read<uint32_t>(get_view_angles + 0x1C);

		// dynamically get the client class head node (@GetAllClasses)
		const auto get_all_classes = globals::process.get_vfunc<uint32_t>(
			globals::engine_client, indices::get_all_classes);
		globals::client_class_head = globals::process.read<uint32_t>(get_all_classes + 0x1);
	}
} // namespace sdk