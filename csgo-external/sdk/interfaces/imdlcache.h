#pragma once

#include "../common.h"
#include "../classes/studio.h"

#include <stdint.h>
#include <epic/read_write_variable.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L169
	class IMDLCache {
	public:
		IMDLCache() noexcept : m_address(0) {}
		explicit IMDLCache(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t imdlcache() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<CUtlMap> m_MDLDict;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L187
		mango::RWVariable<studiohdr_t> get_studio_hdr(const MDLHandle_t handle) const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L191
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L215
		mango::RWVariable<virtualmodel_t> get_virtual_model(const MDLHandle_t handle) const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/datacache/imdlcache.h#L200
		mango::RWVariable<studiodata_t> get_user_data(const MDLHandle_t handle) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk