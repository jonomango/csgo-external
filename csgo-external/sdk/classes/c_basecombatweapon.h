#pragma once

#include "c_econentity.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/basecombatweapon_shared.h#L177
	class C_BaseCombatWeapon : public C_EconEntity {
	public:
		C_BaseCombatWeapon() = default;
		explicit C_BaseCombatWeapon(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_base_combat_weapon_addr() const { return this->m_base_combat_weapon_addr; }

	public:
		mango::RWVariable<float> m_flNextPrimaryAttack;

	private:
		uint32_t m_base_combat_weapon_addr = 0;
	};
} // namespace sdk