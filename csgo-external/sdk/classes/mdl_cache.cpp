#include "mdl_cache.h"

#include "../misc/constants.h"


namespace sdk {
	// get a studiohdr_t* by it's mdl handle
	uint32_t MDLCache::get_studio_hdr(const MDLHandle handle) {
		// NOTE: this isn't fully recreated: i don't think this'll work if the model
		// isn't cached, but its just a bit too time consuming for me to fully rebuild

		// ref https://github.com/VSES/SourceEngine2007/blob/master/src_main/datacache/mdlcache.cpp
		const auto mdldict = globals::process.read<uint32_t>(this->m_address + 0x28);
		const auto studiodata = globals::process.read<uint32_t>(mdldict + (0x10 * handle) + 0xC);
		if (studiodata) {
			// i think this is CheckData(m_MDLDict[handle]->m_MDLCache, MDLCACHE_STUDIOHDR);
			// which might just be a wrapper for reinterpret_cast<studiohdr_t*>(studiodata->m_pUserData)
			return globals::process.read<uint32_t>(studiodata + 0x88);
		}

		return 0;
	}
} // namespace sdk