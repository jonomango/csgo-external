#include "c_baseanimating.h"

#include "../misc/constants.h"


namespace sdk {
	C_BaseAnimating::C_BaseAnimating(const uint32_t address) noexcept : m_base_animating_addr(address), C_BaseEntity(address) {
		this->m_BoneAccessor.setup(globals::process, address + offsets::m_BoneAccessor);
		this->m_nSequence.setup(globals::process, address + offsets::m_nSequence);
	}
} // namespace sdk