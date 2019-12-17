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
		using namespace mango;

		// cache interfaces
		InterfaceCache interface_cache;
		interface_cache.cache();

		// set interfaces
		interfaces::client = CHLClient(interface_cache.get<Fnv1a<uint64_t>("client_panorama.dll:VClient")>());
		interfaces::engine_client = IVEngineClient(interface_cache.get<Fnv1a<uint64_t>("engine.dll:VEngineClient")>());
		interfaces::client_entity_list = IClientEntityList(interface_cache.get<Fnv1a<uint64_t>("client_panorama.dll:VClientEntityList")>());
		interfaces::engine_cvar = ICvar(interface_cache.get<Fnv1a<uint64_t>("vstdlib.dll:VEngineCvar")>());
		interfaces::material_system = IMaterialSystem(interface_cache.get<Fnv1a<uint64_t>("materialsystem.dll:VMaterialSystem")>());
		interfaces::engine_trace = IEngineTrace(interface_cache.get<Fnv1a<uint64_t>("engine.dll:EngineTraceClient")>());
		interfaces::model_info = IVModelInfo(interface_cache.get<Fnv1a<uint64_t>("engine.dll:VModelInfoClient")>());
		interfaces::mdl_cache = IMDLCache(interface_cache.get<Fnv1a<uint64_t>("datacache.dll:MDLCache")>());

		mango::logger.success(enc_str("Found interfaces."));

		// used in IMaterial::GetName and IMaterial::GetTextureGroupName
		const auto get_name_imp = mango::find_pattern(globals::process, 
			enc_str("materialsystem.dll"), enc_str("80 3D ? ? ? ? ? 66 8B 01"));
		globals::cached_strings = globals::process.read<uint32_t>(
			globals::process.read<uint32_t>(get_name_imp + 0x2) + 0x4);

		// get the globalvars address
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/cdll_client_int.cpp#L871
		const auto init = mango::get_vfunc<uint32_t>(
			globals::process, interfaces::client.chlclient(), indices::init);
		const auto globals_vars_addr = globals::process.read<uint32_t>(init + 0x1B);
		globals::global_vars_base.setup(globals::process, globals::process.read<uint32_t>(globals_vars_addr));

		// get the model loader
		const auto get_model_name = mango::get_vfunc<uint32_t>(globals::process, 
			interfaces::model_info.ivmodelinfo(), indices::get_model_name);
		globals::model_loader = IModelLoader(globals::process.read<uint32_t>(get_model_name + 0x17));

		// get the clientmode address
		const auto hud_process_input = mango::get_vfunc<uint32_t>(globals::process, 
			interfaces::client.chlclient(), indices::hud_process_input);
		const auto client_mode_addr = globals::process.read<uint32_t>(hud_process_input + 0x5);
		globals::client_mode = IClientMode(globals::process.read<uint32_t>(client_mode_addr));

		// get the glow object manager address
		const auto do_post_screen_space_effects = mango::get_vfunc<uint32_t>(globals::process, 
			globals::client_mode.iclientmode(), indices::do_post_screen_space_effects);
		const auto get_glow_manager = do_post_screen_space_effects + 0x22 + 0x4 + // jmp is relative to next instruction
			globals::process.read<uint32_t>(do_post_screen_space_effects + 0x22);
		globals::glow_object_manager = CGlowObjectManager(globals::process.read<uint32_t>(get_glow_manager + 0x19));

		// GetLocalPlayer()
		const auto get_local_player = mango::get_vfunc<uint32_t>(globals::process, 
			interfaces::engine_client.ivengineclient(), indices::get_local_player);
		
		// GetLocalPlayer() uses m_nPlayerSlot
		offsets::m_nPlayerSlot = globals::process.read<uint32_t>(get_local_player + 0x16);
		
		// IsInGame() uses m_nSignonState
		const auto is_in_game = mango::get_vfunc<uint32_t>(globals::process, 
			interfaces::engine_client.ivengineclient(), indices::is_in_game);
		offsets::m_nSignonState = globals::process.read<uint32_t>(is_in_game + 0x7);
		
		// GetViewAngles() uses m_vecViewAngles
		const auto get_view_angles = mango::get_vfunc<uint32_t>(globals::process, 
			interfaces::engine_client.ivengineclient(), indices::get_view_angles);
		offsets::m_vecViewAngles = globals::process.read<uint32_t>(get_view_angles + 0x1C);

		// not my favorite method but it works
		if (const auto command_addr = interfaces::engine_cvar.find_command(enc_str("cl_fullupdate"))) {
			const auto command = globals::process.read<ConCommand>(command_addr);
			offsets::m_nDeltaTick = globals::process.read<uint32_t>(command.m_command_callback + 0x17);
		}

		// get client state address from the mov operand
		const auto client_state_addr = globals::process.read<uint32_t>(get_local_player + 0x10);
		globals::client_state = CBaseClientState(globals::process.read<uint32_t>(client_state_addr));
		
		// get netvars
		NetvarCache netvar_cache;
		netvar_cache.cache();
		
		// C_CSPlayer
		offsets::m_angEyeAngles = netvar_cache.get<Fnv1a<uint64_t>("DT_CSPlayer:m_angEyeAngles")>();
		offsets::m_bGunGameImmunity = netvar_cache.get<Fnv1a<uint64_t>("DT_CSPlayer:m_bGunGameImmunity")>();
		offsets::m_flFlashDuration = netvar_cache.get<Fnv1a<uint64_t>("DT_CSPlayer:m_flFlashDuration")>();

		// m_iGlowIndex is set in the C_CSPlayer constructor, right after
		// it calls GetGlowObjectManager() and RegisterGlowObject()
		offsets::m_iGlowIndex = offsets::m_flFlashDuration + 0x18;

		// C_BasePlayer
		offsets::m_iHealth = netvar_cache.get<Fnv1a<uint64_t>("DT_BasePlayer:m_iHealth")>();
		offsets::m_fFlags = netvar_cache.get<Fnv1a<uint64_t>("DT_BasePlayer:m_fFlags")>();

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseplayer.cpp#L215
		offsets::m_nTickBase = netvar_cache.get<Fnv1a<uint64_t>("DT_LocalPlayerExclusive:m_nTickBase")>();
		offsets::m_vecViewOffset = netvar_cache.get<Fnv1a<uint64_t>("DT_LocalPlayerExclusive:m_vecViewOffset[0]")>();
		offsets::m_vecVelocity = netvar_cache.get<Fnv1a<uint64_t>("DT_LocalPlayerExclusive:m_vecVelocity[0]")>();

		// C_BaseAnimating
		offsets::m_BoneAccessor = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseAnimating:m_nForceBone")>() + 0x18;
		offsets::m_nSequence = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseAnimating:m_nSequence")>();
		offsets::m_flPoseParameter = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseAnimating:m_flPoseParameter")>();
		offsets::m_pStudioHdr = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseAnimating:m_bSuppressAnimSounds")>() + 0x2;

		// C_BaseEntity
		offsets::m_bSpotted = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseEntity:m_bSpotted")>();
		offsets::m_iTeamNum = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseEntity:m_iTeamNum")>();
		offsets::m_vecOrigin = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseEntity:m_vecOrigin")>();
		offsets::m_nModelIndex = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseEntity:m_nModelIndex")>();

		// C_BaseCombatCharacter
		offsets::m_hActiveWeapon = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseCombatCharacter:m_hActiveWeapon")>();
		offsets::m_hMyWeapons = netvar_cache.get<Fnv1a<uint64_t>("DT_BaseCombatCharacter:m_hMyWeapons")>();

		// C_WeaponCSBase
		offsets::m_fAccuracyPenalty = netvar_cache.get<Fnv1a<uint64_t>("DT_WeaponCSBase:m_fAccuracyPenalty")>();

		// C_BaseCombatWeapon

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/basecombatweapon_shared.cpp#L2739
		offsets::m_flNextPrimaryAttack = netvar_cache.get<Fnv1a<uint64_t>("DT_LocalActiveWeaponData:m_flNextPrimaryAttack")>();

		mango::logger.success(enc_str("Found netvars."));

		// get datamap fields
		DatamapCache datamap_cache;
		datamap_cache.cache();

		// C_BasePlayer
		offsets::pl = datamap_cache.get<Fnv1a<uint64_t>("C_BasePlayer:pl")>();
		offsets::m_Local = datamap_cache.get<Fnv1a<uint64_t>("C_BasePlayer:m_Local")>();

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_playerlocaldata.h#L22
		offsets::m_aimPunchAngle = datamap_cache.get<Fnv1a<uint64_t>("CPlayerLocalData:m_aimPunchAngle")>();

		mango::logger.success(enc_str("Found datamap fields."));
	}
} // namespace sdk