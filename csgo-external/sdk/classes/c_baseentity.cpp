#include "c_baseentity.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>


namespace sdk {
	C_BaseEntity::C_BaseEntity(const uint32_t address) noexcept : m_address(address), IClientEntity(address) {
		this->m_bSpotted.setup(globals::process, address + offsets::m_bSpotted);
		this->m_iTeamNum.setup(globals::process, address + offsets::m_iTeamNum);
		this->m_fEffects.setup(globals::process, address + offsets::m_fEffects);
		this->m_vecOrigin.setup(globals::process, address + offsets::m_vecOrigin);
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseentity.h#L1049
	void C_BaseEntity::set_model_pointer(const mango::RWVariable<model_t> new_model) const {
		// OLD METHOD
		if (const auto model = this->get_model()) {
			// override our model's mdl_handle with our desired model's mdl handle
			globals::process.write<MDLHandle_t>(uint32_t(&model) +
				offsetof(model_t, studio), new_model().studio);
		}
	}
} // namespace sdk