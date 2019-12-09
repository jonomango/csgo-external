#pragma once

#include "c_baseflex.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_basecombatcharacter.h#L26
	class C_BaseCombatCharacter : public C_BaseFlex {
	public:
		C_BaseCombatCharacter() = default;
		explicit C_BaseCombatCharacter(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_base_combat_character_addr() const { return this->m_base_combat_character_addr; }

	public:
		mango::ReadWriteVariable<uint32_t> m_hActiveWeapon;

	private:
		uint32_t m_base_combat_character_addr = 0;
	};
} // namespace sdk