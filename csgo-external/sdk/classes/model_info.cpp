#include "model_info.h"

#include "../misc/constants.h"
#include "defines.h"


namespace sdk {
	// get the studiohdr_t* for a model_t*
	uint32_t ModelInfo::get_studio_hdr(const uint32_t modelptr) const {
		const auto model = globals::process.read<model_t>(modelptr);
		if (model.modtype != mod_studio)
			return 0;
		return interfaces::mdl_cache.get_studio_hdr(model.mdl_handle);
	}
} // namespace sdk