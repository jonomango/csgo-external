#pragma once

#include <epic/process.h>

#include "../classes/client.h"
#include "../classes/engine_client.h"
#include "../classes/client_entity_list.h"
#include "../classes/engine_cvar.h"
#include "../classes/material_system.h"
#include "../classes/glow_object_manager.h"


namespace sdk {
	void setup_constants();

	namespace globals {
		inline mango::Process process;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/glow_outline_effect.cpp
		inline GlowObjectManager glow_object_manager;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/iclientmode.h
		inline uint32_t client_mode;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/baseclientstate.h
		inline uint32_t client_state;

		inline uint32_t client_class_head;

		// not sure what to name this but whatever
		inline uint32_t material_name_related_var;
	} // namespace globals
	namespace interfaces {
		inline Client client;
		inline EngineClient engine_client;
		inline ClientEntityList client_entity_list;
		inline EngineCvar engine_cvar;
		inline MaterialSystem material_system;
	} // namespace interfaces
	namespace offsets {
		inline uint32_t client_state_local_player_index;
		inline uint32_t client_state_view_angles;

		// https://github.com/VSES/SourceEngine2007/blob/master/src_main/common/protocol.h
		inline uint32_t client_state_signon_state;

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

		// datamap

		// HARDCODED
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
		const inline uint32_t get_all_classes = 8;
		const inline uint32_t hud_process_input = 10;

		// IClientMode
		const inline uint32_t create_move = 24;
		const inline uint32_t do_post_screen_space_effects = 44;

		// IMaterial
		const inline uint32_t get_shader_params = 41;
	} // indices
} // namespace sdk