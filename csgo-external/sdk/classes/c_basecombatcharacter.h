#pragma once

#include "c_baseflex.h"

#include <array>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_basecombatcharacter.h#L26
	class C_BaseCombatCharacter : public C_BaseFlex {
	public:
		C_BaseCombatCharacter() = default;
		explicit C_BaseCombatCharacter(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t cbasecombatcharacter() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<CBaseHandle> m_hActiveWeapon;
		mango::RWVariable<std::array<CBaseHandle, 64>> m_hMyWeapons;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk