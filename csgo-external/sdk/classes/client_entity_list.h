#pragma once

#include <stdint.h>

#include "../misc/interface_base.h"
#include "base_entity.h"


namespace sdk {
	class ClientEntityList : public InterfaceBase<ClientEntityList,
		mango::fnv1a<uint64_t>("client_panorama.dll:VClientEntityList")> {
	public:
		// get an entity from their index
		BaseEntity get_entity(int index) const;

		// get localplayer entity 
		// all this does is call get_entity() with EngineClient::get_local_player_index()
		BaseEntity get_local_player() const;
	};
} // namespace sdk