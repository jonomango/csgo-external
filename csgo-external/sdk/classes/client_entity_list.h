#pragma once

#include <stdint.h>

#include "../misc/interface_base.h"

#include "c_csplayer.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icliententitylist.h
	class ClientEntityList : public InterfaceBase<ClientEntityList,
		mango::fnv1a<uint64_t>("client_panorama.dll:VClientEntityList")> {
	public:
		// get an entity by their index
		template <typename T = IClientEntity>
		T get_client_entity(int index) const { return T(this->get_client_entity_imp(index)); }

		// get localplayer entity 
		// all this does is call get_client_entity() with EngineClient::get_local_player_index()
		C_CSPlayer get_local_player() const;

	private:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icliententitylist.h#L38
		uint32_t get_client_entity_imp(int index) const;
	};
} // namespace sdk