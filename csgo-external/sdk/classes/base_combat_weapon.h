#pragma once

#include "base_entity.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/basecombatweapon_shared.h
	class BaseCombatWeapon : public BaseEntity {
	public:
		BaseCombatWeapon() = default;
		explicit BaseCombatWeapon(const uint32_t address) noexcept { this->m_address = address; }

	public:
		// next time when this weapon is ready to attack
		float get_next_primary_attack() const;

	private:
	};
} // namespace sdk