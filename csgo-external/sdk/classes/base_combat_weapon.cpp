#include "base_combat_weapon.h"

#include "../misc/constants.h"


namespace sdk {
	// next time when this weapon is ready to attack
	float BaseCombatWeapon::get_next_primary_attack() const {
		return globals::process.read<float>(this->m_address + offsets::m_flNextPrimaryAttack);
	}
} // namespace sdk