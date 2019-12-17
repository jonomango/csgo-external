#pragma once

#include "../interfaces/chlclient.h"
#include "../interfaces/ivengineclient.h"
#include "../interfaces/icliententitylist.h"
#include "../interfaces/icvar.h"
#include "../interfaces/imaterialsystem.h"
#include "../interfaces/ienginetrace.h"
#include "../interfaces/ivmodelinfo.h"
#include "../interfaces/imdlcache.h"

#include "../classes/cglowobjectmanager.h"
#include "../classes/imodelloader.h"
#include "../classes/cbaseclientstate.h"
#include "../classes/iclientmode.h"

#include <epic/process.h>
#include <epic/read_write_variable.h>


namespace sdk {
	void setup_constants();

	namespace globals {
		inline mango::Process process;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/globalvars_base.h
		inline mango::RWVariable<CGlobalVarsBase> global_vars_base;
		inline CGlowObjectManager glow_object_manager;
		inline IModelLoader model_loader;
		inline IClientMode client_mode;
		inline CBaseClientState client_state;

		// not sure what this is called but whatever
		inline uint32_t cached_strings;
	} // namespace globals
	namespace interfaces {
		inline CHLClient client;
		inline IVEngineClient engine_client;
		inline IClientEntityList client_entity_list;
		inline ICvar engine_cvar;
		inline IMaterialSystem material_system;
		inline IEngineTrace engine_trace;
		inline IVModelInfo model_info;
		inline IMDLCache mdl_cache;
	} // namespace interfaces
	namespace offsets {
		// dynamic
		inline uint32_t m_iGlowIndex;
		inline uint32_t m_nPlayerSlot;
		inline uint32_t m_nSignonState;
		inline uint32_t m_vecViewAngles;
		inline uint32_t m_nDeltaTick;
		inline uint32_t m_AnimOverlay;
		inline uint32_t m_pStudioHdr;
		inline uint32_t m_BoneAccessor;

		// netvars
		inline uint32_t m_iHealth;
		inline uint32_t m_fFlags;
		inline uint32_t m_bSpotted;
		inline uint32_t m_iTeamNum;
		inline uint32_t m_vecOrigin;
		inline uint32_t m_angEyeAngles;
		inline uint32_t m_bGunGameImmunity;
		inline uint32_t m_flFlashDuration;
		inline uint32_t m_nTickBase;
		inline uint32_t m_hActiveWeapon;
		inline uint32_t m_flNextPrimaryAttack;
		inline uint32_t m_fAccuracyPenalty;
		inline uint32_t m_vecVelocity;
		inline uint32_t m_vecViewOffset;
		inline uint32_t m_nModelIndex;
		inline uint32_t m_hMyWeapons;
		inline uint32_t m_nSequence;
		inline uint32_t m_flPoseParameter;

		// datamap fields
		inline uint32_t pl; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/PlayerState.h
		inline uint32_t m_Local; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_playerlocaldata.h
		inline uint32_t m_aimPunchAngle;	

		// hardcoded
		constexpr inline uint32_t m_bDormant = 0xED; // TODO: this is unneeded... 

	} // namespace offsets
	namespace indices { 
		// VClientEntityList
		constexpr inline uint32_t get_client_entity = 3;

		// VEngineClient
		constexpr inline uint32_t get_local_player = 12;
		constexpr inline uint32_t get_view_angles = 18;
		constexpr inline uint32_t is_in_game = 26;

		// VClient 
		constexpr inline uint32_t init = 0;
		constexpr inline uint32_t get_all_classes = 8;
		constexpr inline uint32_t hud_process_input = 10;

		// IVModelInfo
		constexpr inline uint32_t get_studio_hdr = 32;
		constexpr inline uint32_t get_model_name = 3;

		// EngineTraceClient
		constexpr inline uint32_t trace_ray = 5;

		// IClientMode
		constexpr inline uint32_t do_post_screen_space_effects = 44;
		constexpr inline uint32_t should_draw_crosshair = 28;
		constexpr inline uint32_t should_draw_viewModel = 27;
		constexpr inline uint32_t should_draw_particles = 16;
		constexpr inline uint32_t get_view_model_fov = 35;
		constexpr inline uint32_t should_draw_fog = 17;
		constexpr inline uint32_t create_move = 24;

		// IMaterialSystem
		constexpr inline uint32_t get_num_materials = 90;

		// IMaterial
		constexpr inline uint32_t get_name = 0;
		constexpr inline uint32_t get_texture_group_name = 1;
		constexpr inline uint32_t get_shader_params = 41;

		// IClientRenderable
		constexpr inline uint32_t get_model = 8;

		// IClientNetworkable
		constexpr inline uint32_t get_client_class = 2;
		constexpr inline uint32_t is_dormant = 9;
		constexpr inline uint32_t entindex = 10;

		// CBaseCombatWeapon
		constexpr inline uint32_t get_inaccuracy = 479;

		// IModelLoader
		constexpr inline uint32_t get_count = 2;
	} // indices
} // namespace sdk