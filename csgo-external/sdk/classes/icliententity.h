#pragma once

#include "iclientrenderable.h"


namespace sdk {
	class IClientEntity : public IClientRenderable {
	public:
		IClientEntity() = default;
		explicit IClientEntity(const uint32_t address) noexcept
			: m_client_entity_addr(address), IClientRenderable(address + 4) {}

		// get the underlying address
		uint32_t get_client_entity_addr() const { return this->m_client_entity_addr; }

	private:
		uint32_t m_client_entity_addr = 0;
	};
} // namespace sdk