#pragma once

#include "iclientnetworkable.h"
#include "../common.h"

#include <epic/read_write_variable.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientrenderable.h#L66
	class IClientRenderable : public IClientNetworkable {
	public:
		IClientRenderable() = default;
		explicit IClientRenderable(const uint32_t address) noexcept
			: m_address(address), IClientNetworkable(address + 4) {}

		// get the underlying address
		uint32_t iclientrenderable() const noexcept { return this->m_address; }

	public:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientrenderable.h#L86
		mango::RWVariable<model_t> get_model() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk