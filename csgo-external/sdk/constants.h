#pragma once

#include <epic/process.h>

#include "classes/client.h"
#include "classes/engine_client.h"
#include "classes/client_entity_list.h"


namespace sdk {
	void setup_constants();

	namespace globals {
		inline mango::Process process;

		inline Client client;
		inline EngineClient engine_client;
		inline ClientEntityList client_entity_list;
		inline uint32_t client_state = 0;
	} // namespace globals
	namespace offsets {
		inline uint32_t client_state_local_player_index;
	} // namespace offsets
	namespace indices {
		const inline uint32_t get_local_player_index = 12;
		const inline uint32_t get_client_entity = 3;
	} // indices
} // namespace sdk