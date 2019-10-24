#pragma once

#include <crypto/fnv_hash.h>

#include "interface_cache.h"


namespace sdk {
	template <typename Interface, uint64_t InterfaceHash, int InterfaceVersion = -1>
	class InterfaceBase {
	public:
		static Interface create(const InterfaceCache& cache) {
			Interface iface;
			iface.m_address = cache.get({ InterfaceHash, InterfaceVersion });
			return iface;
		}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	protected:
		uint32_t m_address;
	};
} // namespace sdk