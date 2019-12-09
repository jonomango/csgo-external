#include "constants.h"

#include "interface_cache.h"
#include "netvar_cache.h"
#include "datamap_cache.h"

#include <epic/pattern_scanner.h>
#include <epic/vmt_helpers.h>
#include <misc/fnv_hash.h>
#include <crypto/string_encryption.h>

#include <fstream>


namespace sdk {
	void setup_constants() {
		// cache interfaces
		InterfaceCache interface_cache;
		interface_cache.cache();

		// set interfaces
		interfaces::client.create_interface(interface_cache);
		interfaces::engine_client.create_interface(interface_cache);
		interfaces::client_entity_list.create_interface(interface_cache);
		interfaces::engine_cvar.create_interface(interface_cache);
		interfaces::material_system.create_interface(interface_cache);
		interfaces::engine_trace.create_interface(interface_cache);
		interfaces::model_info.create_interface(interface_cache);
		interfaces::mdl_cache.create_interface(interface_cache);

		mango::logger.success(enc_str("Found interfaces."));

		// ref @IMaterial::GetName and @IMaterial::GetTextureGroupName
		const auto get_name_imp = mango::find_pattern(globals::process, 
			enc_str("materialsystem.dll"), enc_str("80 3D ? ? ? ? ? 66 8B 01"));
		globals::material_name_related_var = globals::process.read<uint32_t>(
			globals::process.read<uint32_t>(get_name_imp + 0x2) + 0x4);

		// dynamically get the globalvars address (ref &Init)
		// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/client/cdll_client_int.cpp#L871
		const auto init = mango::get_vfunc<uint32_t>(
			globals::process, interfaces::client, indices::init);
		const auto globals_vars_addr = globals::process.read<uint32_t>(init + 0x1B);
		globals::global_vars_base = globals::process.read<uint32_t>(globals_vars_addr);

		// dynamically get the clientmode address (ref @HudProcessInput)
		const auto hud_process_input = mango::get_vfunc<uint32_t>(
			globals::process, interfaces::client, indices::hud_process_input);
		const auto client_mode_addr = globals::process.read<uint32_t>(hud_process_input + 0x5);
		globals::client_mode = globals::process.read<uint32_t>(client_mode_addr);

		// dynamically get the glow object manager address (ref @DoPostScreenSpaceEffects)
		const auto do_post_screen_space_effects = mango::get_vfunc<uint32_t>(
			globals::process, globals::client_mode, indices::do_post_screen_space_effects);
		const auto get_glow_manager = do_post_screen_space_effects + 0x22 + 0x4 + // jmp is relative to next instruction
			globals::process.read<uint32_t>(do_post_screen_space_effects + 0x22);
		globals::glow_object_manager = GlowObjectManager(globals::process.read<uint32_t>(get_glow_manager + 0x19));

		// @GetLocalPlayerIndex
		const auto get_local_player_index = mango::get_vfunc<uint32_t>(
			globals::process, interfaces::engine_client, indices::get_local_player_index);
		
		// get client state address from the mov operand (ref @GetLocalPlayerIndex)
		const auto client_state_addr = globals::process.read<uint32_t>(get_local_player_index + 0x10);
		globals::client_state = globals::process.read<uint32_t>(client_state_addr);
		
		// dynamically get the local player index offset (ref @GetLocalPlayerIndex)
		offsets::client_state_local_player_index = 
			globals::process.read<uint32_t>(get_local_player_index + 0x16);
		
		// dynamically get the signon state offset (ref @IsInGame)
		const auto is_in_game = mango::get_vfunc<uint32_t>(
			globals::process, interfaces::engine_client, indices::is_in_game);
		offsets::client_state_signon_state = globals::process.read<uint32_t>(is_in_game + 0x7);
		
		// dynamically get the viewangles offset (ref @GetViewAngles)
		const auto get_view_angles = mango::get_vfunc<uint32_t>(
			globals::process, interfaces::engine_client, indices::get_view_angles);
		offsets::client_state_view_angles = globals::process.read<uint32_t>(get_view_angles + 0x1C);
		
		// dynamically get the client class head node (ref @GetAllClasses)
		const auto get_all_classes = mango::get_vfunc<uint32_t>(
			globals::process, interfaces::client, indices::get_all_classes);
		globals::client_class_head = globals::process.read<uint32_t>(get_all_classes + 0x1);

		using namespace mango;

		// get netvars
		NetvarCache netvar_cache;
		netvar_cache.cache();
		
		// C_CSPlayer
		offsets::m_angEyeAngles = netvar_cache.get<fnv1a<uint64_t>("DT_CSPlayer:m_angEyeAngles")>();
		offsets::m_bGunGameImmunity = netvar_cache.get<fnv1a<uint64_t>("DT_CSPlayer:m_bGunGameImmunity")>();
		offsets::m_flFlashDuration = netvar_cache.get<fnv1a<uint64_t>("DT_CSPlayer:m_flFlashDuration")>();

		// m_iGlowIndex is set in the C_CSPlayer constructor, right after
		// it calls GetGlowObjectManager() and RegisterGlowObject()
		offsets::m_iGlowIndex = offsets::m_flFlashDuration + 0x18;

		// C_BasePlayer
		offsets::m_iHealth = netvar_cache.get<fnv1a<uint64_t>("DT_BasePlayer:m_iHealth")>();
		offsets::m_fFlags = netvar_cache.get<fnv1a<uint64_t>("DT_BasePlayer:m_fFlags")>();

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseplayer.cpp#L215
		offsets::m_nTickBase = netvar_cache.get<fnv1a<uint64_t>("DT_LocalPlayerExclusive:m_nTickBase")>();
		offsets::m_vecViewOffset = netvar_cache.get<fnv1a<uint64_t>("DT_LocalPlayerExclusive:m_vecViewOffset[0]")>();
		offsets::m_vecVelocity = netvar_cache.get<fnv1a<uint64_t>("DT_LocalPlayerExclusive:m_vecVelocity[0]")>();

		// C_BaseAnimating
		offsets::m_BoneAccessor = netvar_cache.get<fnv1a<uint64_t>("DT_BaseAnimating:m_nForceBone")>() + 0x18;

		// C_BaseEntity
		offsets::m_bSpotted = netvar_cache.get<fnv1a<uint64_t>("DT_BaseEntity:m_bSpotted")>();
		offsets::m_iTeamNum = netvar_cache.get<fnv1a<uint64_t>("DT_BaseEntity:m_iTeamNum")>();
		offsets::m_vecOrigin = netvar_cache.get<fnv1a<uint64_t>("DT_BaseEntity:m_vecOrigin")>();

		// C_BaseCombatCharacter
		offsets::m_hActiveWeapon = netvar_cache.get<fnv1a<uint64_t>("DT_BaseCombatCharacter:m_hActiveWeapon")>();

		// C_WeaponCSBase
		offsets::m_fAccuracyPenalty = netvar_cache.get<fnv1a<uint64_t>("DT_WeaponCSBase:m_fAccuracyPenalty")>();

		// C_BaseCombatWeapon

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/basecombatweapon_shared.cpp#L2739
		offsets::m_flNextPrimaryAttack = netvar_cache.get<fnv1a<uint64_t>("DT_LocalActiveWeaponData:m_flNextPrimaryAttack")>();

		mango::logger.success(enc_str("Found netvars."));

		// get datamap fields
		DatamapCache datamap_cache;
		datamap_cache.cache();

		// C_BasePlayer
		offsets::pl = datamap_cache.get<fnv1a<uint64_t>("C_BasePlayer:pl")>();
		offsets::m_Local = datamap_cache.get<fnv1a<uint64_t>("C_BasePlayer:m_Local")>();

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_playerlocaldata.h#L22
		offsets::m_aimPunchAngle = datamap_cache.get<fnv1a<uint64_t>("CPlayerLocalData:m_aimPunchAngle")>();

		mango::logger.success(enc_str("Found datamap fields."));
	}
} // namespace sdk