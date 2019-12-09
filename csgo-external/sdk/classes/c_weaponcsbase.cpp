#include "c_weaponcsbase.h"

#include "../misc/constants.h"


namespace sdk {
	C_WeaponCSBase::C_WeaponCSBase(const uint32_t address) noexcept : m_weaponcs_base_addr(address), C_BaseCombatWeapon(address) {
		this->m_fAccuracyPenalty.setup(globals::process, address + offsets::m_fAccuracyPenalty);
	}
} // namespace sdk