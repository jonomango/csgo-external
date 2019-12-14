#pragma once

#include "c_baseplayer.h"


namespace sdk {
	class C_CSPlayer : public C_BasePlayer {
	public:
		C_CSPlayer() = default;
		explicit C_CSPlayer(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_csplayer_addr() const { return this->m_csplayer_addr; }

	public:
		mango::RWVariable<bool> m_bGunGameImmunity;
		mango::RWVariable<mango::Vec3f> m_angEyeAngles;
		mango::RWVariable<float> m_flFlashDuration;
		mango::RWVariable<int> m_iGlowIndex;

	private:
		uint32_t m_csplayer_addr = 0;
	};
} // namespace sdk