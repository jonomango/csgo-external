#include "icliententitylist.h"

#include "../misc/constants.h"


namespace sdk {
	// get localplayer entity 
	// all this does is call get_client_entity() with EngineClient::get_local_player_index()
	C_CSPlayer IClientEntityList::get_local_player() const {
		return this->get_client_entity<C_CSPlayer>(interfaces::engine_client.get_local_player());
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icliententitylist.h#L38
	uint32_t IClientEntityList::get_client_entity_imp(const int index) const {
		const auto address = this->m_address + (index + 0xFFFF'DFFF) * 0x10;
		return globals::process.read<uint32_t>(address);
	}
} // namespace sdk