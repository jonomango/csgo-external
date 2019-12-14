#pragma once

#include "c_baseanimatingoverlay.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseflex.h#L126
	class C_BaseFlex : public C_BaseAnimatingOverlay {
	public:
		C_BaseFlex() = default;
		explicit C_BaseFlex(const uint32_t address) noexcept
			: m_base_flex_addr(address), C_BaseAnimatingOverlay(address) {}

		// get the underlying address
		uint32_t get_base_flex_addr() const { return this->m_base_flex_addr; }

	private:
		uint32_t m_base_flex_addr = 0;
	};
} // namespace sdk