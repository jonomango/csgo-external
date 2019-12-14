#include "imdlcache.h"

#include "../misc/constants.h"


namespace sdk {
	IMDLCache::IMDLCache(const uint32_t address) noexcept : m_address(address) {
		this->m_MDLDict.setup(globals::process, address + 0x28);
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L187
	mango::RWVariable<studiohdr_t> IMDLCache::get_studio_hdr(const MDLHandle_t handle) const {
		if (const auto studiodata = this->get_user_data(handle))
			return { globals::process, studiodata().m_pUserData };
		return {};
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L191
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L215
	mango::RWVariable<virtualmodel_t> IMDLCache::get_virtual_model(const MDLHandle_t handle) const {
		if (const auto studiodata = this->get_user_data(handle))
			return { globals::process, studiodata().m_pVirtualModel };
		return {};
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L200
	mango::RWVariable<studiodata_t> IMDLCache::get_user_data(const MDLHandle_t handle) const {
		// user-error
		if (handle == MDLHANDLE_INVALID)
			return {};
		return { globals::process, globals::process.read<uint32_t>(this->m_MDLDict().m_pData + (0x10 * handle) + 0xC) };
	}
} // namespace sdk