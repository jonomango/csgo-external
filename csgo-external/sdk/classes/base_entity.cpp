#include "base_entity.h"

#include "../constants.h"


namespace sdk {
	// get entity health
	int32_t BaseEntity::get_health() const {
		return globals::process.read<int32_t>(this->m_address + 0x100);
	}
} // namespace sdk