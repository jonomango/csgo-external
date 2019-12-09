#include "c_csplayer.h"

#include "../misc/constants.h"


namespace sdk {
	C_CSPlayer::C_CSPlayer(const uint32_t address) noexcept : m_csplayer_addr(address), C_BasePlayer(address) {
		this->m_angEyeAngles.setup(globals::process, address + offsets::m_angEyeAngles);
		this->m_bGunGameImmunity.setup(globals::process, address + offsets::m_bGunGameImmunity);
		this->m_flFlashDuration.setup(globals::process, address + offsets::m_flFlashDuration);
		this->m_iGlowIndex.setup(globals::process, address + offsets::m_iGlowIndex);
	}
} // namespace sdk