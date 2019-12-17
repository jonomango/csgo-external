#pragma once

#include "c_econentity.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/basecombatweapon_shared.h#L177
	class C_BaseCombatWeapon : public C_EconEntity {
	public:
		C_BaseCombatWeapon() = default;
		explicit C_BaseCombatWeapon(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t cbasecombatweapon() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<float> m_flNextPrimaryAttack;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk