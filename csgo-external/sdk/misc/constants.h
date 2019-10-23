#pragma once

#include <epic/process.h>

#include "../classes/client.h"
#include "../classes/engine_client.h"
#include "../classes/client_entity_list.h"


namespace sdk {
	void setup_constants();

	namespace globals {
		inline mango::Process process;

		inline uint32_t client_state;
		inline uint32_t client_class_head;
	} // namespace globals
	namespace interfaces {
		inline Client client;
		inline EngineClient engine_client;
		inline ClientEntityList client_entity_list;
	} // namespace interfaces
	namespace offsets {
		inline uint32_t client_state_local_player_index;
		inline uint32_t client_state_view_angles;

		// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/engine/baseclientstate.h
		// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/common/protocol.h
		inline uint32_t client_state_signon_state;
	} // namespace offsets
	namespace netvars { // these are dynamic
		inline uint32_t m_iHealth;
	}
	namespace indices { 
		const inline uint32_t get_local_player_index = 12;
		const inline uint32_t get_client_entity = 3;
		const inline uint32_t is_in_game = 26;
		const inline uint32_t get_view_angles = 18;
		const inline uint32_t get_all_classes = 8;
	} // indices
} // namespace sdk