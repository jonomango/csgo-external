#include "c_basecombatcharacter.h"

#include "../misc/constants.h"


namespace sdk {
	C_BaseCombatCharacter::C_BaseCombatCharacter(const uint32_t address) noexcept : m_base_combat_character_addr(address), C_BaseFlex(address) {
		this->m_hActiveWeapon.setup(globals::process, address + offsets::m_hActiveWeapon);
	}
} // namespace sdk