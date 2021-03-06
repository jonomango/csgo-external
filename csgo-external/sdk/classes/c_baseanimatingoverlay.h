#pragma once

#include "c_baseanimating.h"
#include "../common.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseanimatingoverlay.h#L19
	class C_BaseAnimatingOverlay : public C_BaseAnimating {
	public:
		C_BaseAnimatingOverlay() = default;
		explicit C_BaseAnimatingOverlay(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t cbaseanimatingoverlay() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<CUtlVector<C_AnimationLayer>> m_AnimOverlay;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk