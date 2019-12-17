#include "c_baseplayer.h"

#include "../misc/constants.h"


namespace sdk {
	C_BasePlayer::C_BasePlayer(const uint32_t address) noexcept : m_address(address), C_BaseCombatCharacter(address) {
		this->m_iHealth.setup(globals::process, address + offsets::m_iHealth);
		this->m_fFlags.setup(globals::process, address + offsets::m_fFlags);
		this->m_nTickBase.setup(globals::process, address + offsets::m_nTickBase);
		this->m_vecVelocity.setup(globals::process, address + offsets::m_vecVelocity);
		this->m_vecViewOffset.setup(globals::process, address + offsets::m_vecViewOffset);
		this->m_aimPunchAngle.setup(globals::process, address + offsets::m_Local + offsets::m_aimPunchAngle);
	}
} // namespace sdk