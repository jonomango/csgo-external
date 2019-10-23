#include "base_entity.h"

#include "../misc/constants.h"


namespace sdk {
	// get entity health
	int32_t BaseEntity::get_health() const {
		return globals::process.read<int32_t>(this->m_address + netvars::m_iHealth);
	}
} // namespace sdk