#include "ivengineclient.h"

#include "../misc/constants.h"


namespace sdk {
	// // https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L243
	int IVEngineClient::get_local_player() const {
		return globals::client_state.m_nPlayerSlot() + 1;
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L282
	bool IVEngineClient::is_in_game() const {
		// https://github.com/VSES/SourceEngine2007/blob/master/src_main/common/protocol.h#L159
		return globals::client_state.m_nSignonState() == 6; // SIGNONSTATE_FULL
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L263
	mango::Vec3f IVEngineClient::get_view_angles() const {
		return globals::client_state.m_vecViewAngles();
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L265
	void IVEngineClient::set_view_angles(const mango::Vec3f& angles) const {
		globals::client_state.m_vecViewAngles = angles;
	}
} // namespace sdk