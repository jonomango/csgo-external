#include "c_baseanimating.h"

#include "../misc/constants.h"


namespace sdk {
	C_BaseAnimating::C_BaseAnimating(const uint32_t address) noexcept : m_address(address), C_BaseEntity(address) {
		this->m_nSequence.setup(globals::process, address + offsets::m_nSequence);
		this->m_BoneAccessor.setup(globals::process, address + offsets::m_BoneAccessor);
		this->m_flPoseParameter.setup(globals::process, address + offsets::m_flPoseParameter);
	}

	// https://github.com/FWGS/source-sdk-2013-tegra/blob/master/sp/src/game/client/c_baseanimating.h#L174
	mango::RWVariable<CStudioHdr> C_BaseAnimating::get_model_ptr() const {
		return { globals::process, globals::process.read<uint32_t>(this->m_address + offsets::m_pStudioHdr) };
	}
} // namespace sdk