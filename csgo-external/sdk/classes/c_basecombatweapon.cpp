#include "c_basecombatweapon.h"

#include "../misc/constants.h"


namespace sdk {
	C_BaseCombatWeapon::C_BaseCombatWeapon(const uint32_t address) noexcept : m_address(address), C_EconEntity(address) {
		this->m_flNextPrimaryAttack.setup(globals::process, address + offsets::m_flNextPrimaryAttack);
	}
} // namespace sdk