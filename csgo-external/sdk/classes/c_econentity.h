#pragma once

#include "c_baseflex.h"


namespace sdk {
	class C_EconEntity : public C_BaseFlex {
	public:
		C_EconEntity() = default;
		explicit C_EconEntity(const uint32_t address) noexcept
			: m_address(address), C_BaseFlex(address) {}

		// get the underlying address
		uint32_t ceconentity() const noexcept { return this->m_address; }

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk