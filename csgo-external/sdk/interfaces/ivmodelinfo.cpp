#include "ivmodelinfo.h"

#include "../misc/constants.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/ivmodelinfo.h#L135
	mango::RWVariable<virtualmodel_t> IVModelInfo::get_virtual_model(const mango::RWVariable<studiohdr_t> studiohdr) const {
		// nice
		if (!studiohdr)
			return {};

		// to get the studiohdr2
		const auto offset = studiohdr().studiohdr2index;
		if (!offset)
			return {};

		const auto studiohdr2 = globals::process.read<studiohdr2_t>(&studiohdr + offset);
		return interfaces::mdl_cache.get_virtual_model(studiohdr2.m_VirtualModelMDL);
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/ivmodelinfo.h#L146
	mango::RWVariable<studiohdr_t> IVModelInfo::get_studio_hdr(const mango::RWVariable<model_t> model) const {
		// nice
		if (!model)
			return {};

		// read the model_t struct, but only once
		const auto m = model();

		// nice
		if (m.type != modtype_t::mod_studio)
			return {};

		// forward to IMDLCache::GetStudioHdr()
		return interfaces::mdl_cache.get_studio_hdr(m.studio);
	}
} // namespace sdk