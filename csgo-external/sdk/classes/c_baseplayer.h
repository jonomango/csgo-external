#pragma once

#include "c_basecombatcharacter.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseplayer.h#L71
	class C_BasePlayer : public C_BaseCombatCharacter {
	public:
		C_BasePlayer() = default;
		explicit C_BasePlayer(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_base_player_addr() const { return this->m_base_player_addr; }

	public:
		mango::RWVariable<int> m_iHealth;
		mango::RWVariable<int> m_fFlags;
		mango::RWVariable<int> m_nTickBase;
		mango::RWVariable<mango::Vec3f> m_vecVelocity;
		mango::RWVariable<mango::Vec3f> m_vecViewOffset;
		mango::RWVariable<mango::Vec3f> m_aimPunchAngle;

	private:
		uint32_t m_base_player_addr = 0;
	};
} // namespace sdk