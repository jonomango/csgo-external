#pragma once

#include "base_entity.h"

#include <epic/read_write_variable.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/basecombatweapon_shared.h
	// this actually inherits from CBaseAnimating but whatever
	class BaseCombatWeapon : public BaseEntity {
	public:
		BaseCombatWeapon() = default;
		explicit BaseCombatWeapon(const uint32_t address) noexcept;

	public:
		mango::ReadWriteVariable<float> m_flNextPrimaryAttack;
	};
} // namespace sdk