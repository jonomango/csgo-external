#pragma once

#include "c_baseentity.h"
#include "studio.h"

#include <array>


namespace sdk {
	class C_BaseAnimating : public C_BaseEntity {
	public:
		C_BaseAnimating() = default;
		explicit C_BaseAnimating(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t cbaseanimating() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<int> m_nSequence;
		mango::RWVariable<CBoneAccessor> m_BoneAccessor;
		mango::RWVariable<std::array<float, 24>> m_flPoseParameter;

		// https://github.com/FWGS/source-sdk-2013-tegra/blob/master/sp/src/game/client/c_baseanimating.h#L174
		mango::RWVariable<CStudioHdr> get_model_ptr() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace mango