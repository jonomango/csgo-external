#include "client_entity_list.h"

#include "../misc/constants.h"


namespace sdk {
	// get an entity from their index
	BaseEntity ClientEntityList::get_entity(uint32_t index) const {
		// ref @GetClientEntity
		index += 0x0FFFFDFFF;
		const auto address = interfaces::client_entity_list + (index + index) * 0x8;
		return BaseEntity(globals::process.read<uint32_t>(address));
	}

	// get localplayer entity 
	// all this does is call get_entity() with EngineClient::get_local_player_index()
	BaseEntity ClientEntityList::get_local_player() const {
		return this->get_entity(interfaces::engine_client.get_local_player());
	}
} // namespace sdk