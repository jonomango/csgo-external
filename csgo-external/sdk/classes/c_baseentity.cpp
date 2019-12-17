#include "c_baseentity.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>


namespace sdk {
	C_BaseEntity::C_BaseEntity(const uint32_t address) noexcept : m_address(address), IClientEntity(address) {
		this->m_bSpotted.setup(globals::process, address + offsets::m_bSpotted);
		this->m_iTeamNum.setup(globals::process, address + offsets::m_iTeamNum);
		this->m_vecOrigin.setup(globals::process, address + offsets::m_vecOrigin);
	}
} // namespace sdk