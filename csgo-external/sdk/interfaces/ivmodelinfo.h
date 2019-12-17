#pragma once

#include "../common.h"
#include "../classes/studio.h"

#include <stdint.h>
#include <epic/read_write_variable.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/ivmodelinfo.h#L101
	class IVModelInfo {
	public:
		IVModelInfo() noexcept : m_address(0) {}
		explicit IVModelInfo(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		uint32_t ivmodelinfo() const noexcept { return this->m_address; }

	public:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/ivmodelinfo.h#L135
		mango::RWVariable<virtualmodel_t> get_virtual_model(const mango::RWVariable<studiohdr_t> studiohdr) const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/ivmodelinfo.h#L146
		mango::RWVariable<studiohdr_t> get_studio_hdr(const mango::RWVariable<model_t> model) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk