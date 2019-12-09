#pragma once

#include "c_basecombatweapon.h"


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/cstrike/weapon_csbase.h#L78
	class C_WeaponCSBase : public C_BaseCombatWeapon {
	public:
		C_WeaponCSBase() = default;
		explicit C_WeaponCSBase(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_weaponcs_base_addr() const { return this->m_weaponcs_base_addr; }

	public:
		mango::ReadWriteVariable<float> m_fAccuracyPenalty;

	private:
		uint32_t m_weaponcs_base_addr = 0;
	};
} // namespace sdk