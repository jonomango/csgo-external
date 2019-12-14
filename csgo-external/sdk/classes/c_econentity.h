#pragma once

#include "c_baseflex.h"


namespace sdk {
	class C_EconEntity : public C_BaseFlex {
	public:
		C_EconEntity() = default;
		explicit C_EconEntity(const uint32_t address) noexcept
			: m_econ_entity_addr(address), C_BaseFlex(address) {}

		// get the underlying address
		uint32_t get_econ_entity_addr() const { return this->m_econ_entity_addr; }

	private:
		uint32_t m_econ_entity_addr = 0;
	};
} // namespace sdk