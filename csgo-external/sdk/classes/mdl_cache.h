#pragma once

#include "../misc/interface_base.h"

#include "defines.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L169
	class MDLCache : public InterfaceBase<MDLCache,
		mango::fnv1a<uint64_t>("datacache.dll:MDLCache")> {
	public:

		// get a studiohdr_t* by it's mdl handle
		uint32_t get_studio_hdr(const MDLHandle handle);
	};
} // namespace sdk