#pragma once

#include <epic/process.h>

#include "../classes/client.h"
#include "../classes/engine_client.h"
#include "../classes/client_entity_list.h"
#include "../classes/engine_cvar.h"
#include "../classes/material_system.h"
#include "../classes/glow_object_manager.h"
#include "../classes/engine_trace.h"
#include "../classes/model_info.h"
#include "../classes/mdl_cache.h"


namespace sdk {
	void setup_constants();

	namespace globals {
		inline mango::Process process;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/globalvars_base.h
		inline uint32_t global_vars_base;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/glow_outline_effect.cpp
		inline GlowObjectManager glow_object_manager;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/iclientmode.h
		inline uint32_t client_mode;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/baseclientstate.h
		inline uint32_t client_state;

		// the head node to a linked list of ClientClass pointers
		inline uint32_t client_class_head;

		// not sure what this is but whatever
		inline uint32_t material_name_related_var;
	} // namespace globals
	namespace interfaces {
		inline Client client;
		inline EngineClient engine_client;
		inline ClientEntityList client_entity_list;
		inline EngineCvar engine_cvar;
		inline MaterialSystem material_system;
		inline EngineTrace engine_trace;
		inline ModelInfo model_info;
		inline MDLCache mdl_cache;
	} // namespace interfaces
	namespace offsets {
		inline uint32_t client_state_local_player_index;
		inline uint32_t client_state_view_angles;

		// https://github.com/VSES/SourceEngine2007/blob/master/src_main/common/protocol.h
		inline uint32_t client_state_signon_state;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseanimating.h#L518
		inline uint32_t m_BoneAccessor;

		inline uint32_t m_iGlowIndex;

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

		// datamap fields
		inline uint32_t m_Local;
		inline uint32_t m_vecViewOffset;
		inline uint32_t m_aimPunchAngle;	

		// hardcoded
		const inline uint32_t m_bDormant = 0xED;

	} // namespace offsets
	namespace indices { 
		// VClientEntityList
		const inline uint32_t get_client_entity = 3;

		// VEngineClient
		const inline uint32_t get_local_player_index = 12;
		const inline uint32_t get_view_angles = 18;
		const inline uint32_t is_in_game = 26;

		// VClient 
		const inline uint32_t init = 0;
		const inline uint32_t get_all_classes = 8;
		const inline uint32_t hud_process_input = 10;

		// EngineTraceClient
		const inline uint32_t trace_ray = 5;

		// IClientMode
		const inline uint32_t create_move = 24;
		const inline uint32_t do_post_screen_space_effects = 44;

		// IMaterial
		const inline uint32_t get_shader_params = 41;

		// IClientRenderable
		const inline uint32_t get_model = 8;
	} // indices
} // namespace sdk