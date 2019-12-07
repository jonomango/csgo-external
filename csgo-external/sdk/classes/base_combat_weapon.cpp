#include "base_combat_weapon.h"

#include "../misc/constants.h"


namespace sdk {
	BaseCombatWeapon::BaseCombatWeapon(const uint32_t address) noexcept {
		this->m_address = address;

		this->m_flNextPrimaryAttack.setup(globals::process, this->m_address + offsets::m_flNextPrimaryAttack);
	}
} // namespace sdk