#include "constants.h"

#include "interface_cache.h"
#include "netvar_cache.h"
#include "datamap_cache.h"

#include <epic/pattern_scanner.h>
#include <crypto/fnv_hash.h>
#include <crypto/string_encryption.h>


namespace sdk {
	void setup_constants() {
		// cache interfaces
		InterfaceCache interface_cache;
		interface_cache.cache();

		// set interfaces
		interfaces::client = Client::create(interface_cache);
		interfaces::engine_client = EngineClient::create(interface_cache);
		interfaces::client_entity_list = ClientEntityList::create(interface_cache);
		
		mango::logger.success(enc_str("Found interfaces."));

		// @GetLocalPlayerIndex
		const auto get_local_player_index = globals::process.get_vfunc<uint32_t>(
			interfaces::engine_client, indices::get_local_player_index);
		
		// get client state address from the mov operand (ref @GetLocalPlayerIndex)
		const auto client_state_addr = globals::process.read<uint32_t>(get_local_player_index + 0x10);
		globals::client_state = globals::process.read<uint32_t>(client_state_addr);
		
		// dynamically get the local player index offset (ref @GetLocalPlayerIndex)
		offsets::client_state_local_player_index = 
			globals::process.read<uint32_t>(get_local_player_index + 0x16);
		
		// dynamically get the signon state offset (ref @IsInGame)
		const auto is_in_game = globals::process.get_vfunc<uint32_t>(
			interfaces::engine_client, indices::is_in_game);
		offsets::client_state_signon_state = globals::process.read<uint32_t>(is_in_game + 0x7);
		
		// dynamically get the viewangles offset (ref @GetViewAngles)
		const auto get_view_angles = globals::process.get_vfunc<uint32_t>(
			interfaces::engine_client, indices::get_view_angles);
		offsets::client_state_view_angles = globals::process.read<uint32_t>(get_view_angles + 0x1C);
		
		// dynamically get the client class head node (ref @GetAllClasses)
		const auto get_all_classes = globals::process.get_vfunc<uint32_t>(
			interfaces::client, indices::get_all_classes);
		globals::client_class_head = globals::process.read<uint32_t>(get_all_classes + 0x1);

		using namespace mango;

		// cache netvars
		NetvarCache netvar_cache;
		netvar_cache.cache();
		
		// get netvars
		offsets::m_iHealth = netvar_cache.get<fnv1a<uint64_t>("DT_BasePlayer:m_iHealth")>();
		offsets::m_fFlags = netvar_cache.get<fnv1a<uint64_t>("DT_BasePlayer:m_fFlags")>();
		offsets::m_bSpotted = netvar_cache.get<fnv1a<uint64_t>("DT_BaseEntity:m_bSpotted")>();
		offsets::m_iTeamNum = netvar_cache.get<fnv1a<uint64_t>("DT_BaseEntity:m_iTeamNum")>();
		offsets::m_vecOrigin = netvar_cache.get<fnv1a<uint64_t>("DT_BaseEntity:m_vecOrigin")>();
		offsets::m_angEyeAngles = netvar_cache.get<fnv1a<uint64_t>("DT_CSPlayer:m_angEyeAngles")>();
		offsets::m_bGunGameImmunity = netvar_cache.get<fnv1a<uint64_t>("DT_CSPlayer:m_bGunGameImmunity")>();
		offsets::m_flFlashDuration = netvar_cache.get<fnv1a<uint64_t>("DT_CSPlayer:m_flFlashDuration")>();
		
		mango::logger.success(enc_str("Found netvars."));

		// cache datamap fields
		DatamapCache datamap_cache;
		datamap_cache.cache();

		// get datamap fields


		mango::logger.success(enc_str("Found datamap fields."));
	}
} // namespace sdk