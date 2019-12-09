#pragma once

#include "c_baseentity.h"


namespace sdk {
	class C_BaseAnimating : public C_BaseEntity {
	public:
		C_BaseAnimating() = default;
		explicit C_BaseAnimating(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_base_animating_addr() const { return this->m_base_animating_addr; }

	public:
		mango::ReadWriteVariable<BoneAccessor> m_BoneAccessor;

	private:
		uint32_t m_base_animating_addr = 0;
	};
} // namespace mango