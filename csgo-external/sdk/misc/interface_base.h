#pragma once

#include <crypto/fnv_hash.h>

#include "interface_cache.h"


namespace sdk {
	template <typename Interface, uint64_t InterfaceHash, int InterfaceVersion = -1>
	class InterfaceBase {
	public:
		static Interface create(const InterfaceCache& cache) {
			Interface created_interface;
			created_interface.m_address = cache.get({ InterfaceHash, InterfaceVersion });
			return created_interface;
		}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	protected:
		uint32_t m_address;
	};
} // namespace sdk