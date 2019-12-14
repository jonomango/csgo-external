#pragma once

#include "c_baseanimating.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseanimatingoverlay.h#L19
	class C_BaseAnimatingOverlay : public C_BaseAnimating {
	public:
		C_BaseAnimatingOverlay() = default;
		explicit C_BaseAnimatingOverlay(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_base_animating_overlay_addr() const { return this->m_base_animating_overlay_addr; }

	public:
		mango::RWVariable<CUtlVector> m_AnimOverlay;

	private:
		uint32_t m_base_animating_overlay_addr = 0;
	};
} // namespace sdk