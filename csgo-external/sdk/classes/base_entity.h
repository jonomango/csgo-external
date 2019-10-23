#pragma once

#include <stdint.h>


namespace sdk {
	class BaseEntity {
	public:
		BaseEntity() = default;
		explicit BaseEntity(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		// get entity health
		int32_t get_health() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk