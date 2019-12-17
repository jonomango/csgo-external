#pragma once

#include "iclientrenderable.h"


namespace sdk {
	class IClientEntity : public IClientRenderable {
	public:
		IClientEntity() = default;
		explicit IClientEntity(const uint32_t address) noexcept
			: m_address(address), IClientRenderable(address + 4) {}

		// get the underlying address
		uint32_t icliententity() const noexcept { return this->m_address; }

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk