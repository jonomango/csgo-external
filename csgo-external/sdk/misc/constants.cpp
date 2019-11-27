#include "constants.h"

#include "interface_cache.h"
#include "netvar_cache.h"
#include "datamap_cache.h"

#include <epic/pattern_scanner.h>
#include <misc/fnv_hash.h>
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
		interfaces::engine_cvar = EngineCvar::create(interface_cache);
		interfaces::material_system = MaterialSystem::create(interface_cache);
		
		mango::logger.success(enc_str("Found interfaces."));

		// ref @IMaterial::GetName and @IMaterial::GetTextureGroupName
		const auto get_name_imp = mango::find_pattern(globals::process, 
			enc_str("materialsystem.dll"), enc_str("80 3D ? ? ? ? ? 66 8B 01"));
		globals::material_name_related_var = globals::process.read<uint32_t>(
			globals::process.read<uint32_t>(get_name_imp + 0x2) + 0x4);

		// dynamically get the clientmode address (ref @HudProcessInput)
		const auto hud_process_input = globals::process.get_vfunc<uint32_t>(
			interfaces::client, indices::hud_process_input);
		const auto client_mode_addr = globals::process.read<uint32_t>(hud_process_input + 0x5);
		globals::client_mode = globals::process.read<uint32_t>(client_mode_addr);

		// dynamically get the glow object manager address (ref @DoPostScreenSpaceEffects)
		const auto do_post_screen_space_effects = globals::process.get_vfunc<uint32_t>(
			globals::client_mode, indices::do_post_screen_space_effects);
		const auto get_glow_manager = do_post_screen_space_effects + 0x22 + 4 + // jmp is relative to next instruction
			globals::process.read<uint32_t>(do_post_screen_space_effects + 0x22); // relative jmp
		globals::glow_object_manager = GlowObjectManager(globals::process.read<uint32_t>(get_glow_manager + 0x19));

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

		// m_iGlowIndex is set in the CCSPlayer constructor, right after
		// it calls GetGlowObjectManager() and RegisterGlowObject()
		offsets::m_iGlowIndex = offsets::m_flFlashDuration + 0x18;

		mango::logger.success(enc_str("Found netvars."));

		// cache datamap fields
		DatamapCache datamap_cache;
		datamap_cache.cache();

		// get datamap fields
		offsets::m_vecViewOffset = datamap_cache.get<fnv1a<uint64_t>("C_BaseEntity:m_vecViewOffset")>();

		mango::logger.success(enc_str("Found datamap fields."));
	}
} // namespace sdk