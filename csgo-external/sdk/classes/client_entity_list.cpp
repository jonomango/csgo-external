#include "client_entity_list.h"

#include "../misc/constants.h"


namespace sdk {
	// get localplayer entity 
	// all this does is call get_client_entity() with EngineClient::get_local_player_index()
	C_CSPlayer ClientEntityList::get_local_player() const {
		return this->get_client_entity<C_CSPlayer>(interfaces::engine_client.get_local_player());
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icliententitylist.h#L38
	uint32_t ClientEntityList::get_client_entity_imp(int index) const {
		index += 0xFFFF'DFFF;
		const auto address = interfaces::client_entity_list + (index + index) * 0x8;
		return globals::process.read<uint32_t>(address);
	}
} // namespace sdk