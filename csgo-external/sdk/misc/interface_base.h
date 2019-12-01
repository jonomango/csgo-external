#pragma once

#include <misc/fnv_hash.h>

#include "interface_cache.h"


namespace sdk {
	template <typename Interface, uint64_t InterfaceHash, int InterfaceVersion = -1>
	class InterfaceBase {
	public:
		void create_interface(const InterfaceCache& cache) {
			this->m_address = cache.get({ InterfaceHash, InterfaceVersion });
		}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	protected:
		uint32_t m_address = 0;
	};
} // namespace sdk