#include "c_baseanimatingoverlay.h"

#include "../misc/constants.h"


namespace sdk {
	C_BaseAnimatingOverlay::C_BaseAnimatingOverlay(const uint32_t address) noexcept : m_base_animating_overlay_addr(address), C_BaseAnimating(address) {
		this->m_AnimOverlay.setup(globals::process, address + 0x2980);
	}
} // namespace sdk