#include "cbaseclientstate.h"

#include "../misc/constants.h"


namespace sdk {
	CBaseClientState::CBaseClientState(const uint32_t address) noexcept : m_address(address) {
		this->m_nSignonState.setup(globals::process, address + offsets::m_nSignonState);
		this->m_nDeltaTick.setup(globals::process, address + offsets::m_nDeltaTick);
		this->m_nPlayerSlot.setup(globals::process, address + offsets::m_nPlayerSlot);
		this->m_vecViewAngles.setup(globals::process, address + offsets::m_vecViewAngles);
	}

	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/baseclientstate.h#L150
	void CBaseClientState::force_full_update() const {
		this->m_nDeltaTick = -1;
	}
} // namespace sdk