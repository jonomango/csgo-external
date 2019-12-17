#pragma once

#include "c_basecombatweapon.h"


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/cstrike/weapon_csbase.h#L78
	class C_WeaponCSBase : public C_BaseCombatWeapon {
	public:
		C_WeaponCSBase() = default;
		explicit C_WeaponCSBase(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t cweaponcsbase() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<float> m_fAccuracyPenalty;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk