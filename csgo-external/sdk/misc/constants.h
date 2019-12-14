#pragma once

#include <epic/process.h>

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


namespace sdk {
	void setup_constants();

	namespace globals {
		inline mango::Process process;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/globalvars_base.h
		inline uint32_t global_vars_base;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/glow_outline_effect.cpp
		inline CGlowObjectManager glow_object_manager;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L24
		inline IModelLoader model_loader;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/iclientmode.h
		inline uint32_t client_mode;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/baseclientstate.h
		inline CBaseClientState client_state;

		// the head node to a linked list of ClientClass pointers
		inline uint32_t client_class_head;

		// not sure what this is but whatever
		inline uint32_t material_name_related_var;
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
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseanimating.h#L518
		inline uint32_t m_BoneAccessor;

		// dynamic
		inline uint32_t m_iGlowIndex;
		inline uint32_t m_nPlayerSlot;
		inline uint32_t m_nSignonState;
		inline uint32_t m_vecViewAngles;
		inline uint32_t m_nDeltaTick;
		inline uint32_t m_AnimOverlay;

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

		// datamap fields
		inline uint32_t pl; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/PlayerState.h
		inline uint32_t m_Local; // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_playerlocaldata.h
		inline uint32_t m_aimPunchAngle;	

		// hardcoded
		const inline uint32_t m_bDormant = 0xED; // TODO: this is unneeded... 

	} // namespace offsets
	namespace indices { 
		// VClientEntityList
		const inline uint32_t get_client_entity = 3;

		// VEngineClient
		const inline uint32_t get_local_player = 12;
		const inline uint32_t get_view_angles = 18;
		const inline uint32_t is_in_game = 26;

		// VClient 
		const inline uint32_t init = 0;
		const inline uint32_t get_all_classes = 8;
		const inline uint32_t hud_process_input = 10;

		// IVModelInfo
		const inline uint32_t get_studio_hdr = 32;
		const inline uint32_t get_model_name = 3;

		// EngineTraceClient
		const inline uint32_t trace_ray = 5;

		// IClientMode
		const inline uint32_t create_move = 24;
		const inline uint32_t do_post_screen_space_effects = 44;

		// IMaterialSystem
		const inline uint32_t get_num_materials = 90;

		// IMaterial
		const inline uint32_t get_name = 0;
		const inline uint32_t get_texture_group_name = 1;
		const inline uint32_t get_shader_params = 41;

		// IClientRenderable
		const inline uint32_t get_model = 8;

		// IClientNetworkable
		const inline uint32_t get_client_class = 2;
		const inline uint32_t is_dormant = 9;
		const inline uint32_t entindex = 10;

		// CBaseCombatWeapon
		const inline uint32_t get_inaccuracy = 479;

		// IModelLoader
		const inline uint32_t get_count = 2;
	} // indices
} // namespace sdk