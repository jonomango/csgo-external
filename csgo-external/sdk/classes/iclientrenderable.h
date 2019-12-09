#pragma once

#include "iclientnetworkable.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientrenderable.h#L66
	class IClientRenderable : public IClientNetworkable {
	public:
		IClientRenderable() = default;
		explicit IClientRenderable(const uint32_t address) noexcept
			: m_client_renderable_addr(address), IClientNetworkable(address + 4) {}

		// get the underlying address
		uint32_t get_client_renderable_addr() const { return this->m_client_renderable_addr; }

	public:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientrenderable.h#L86
		uint32_t get_model() const;

	private:
		uint32_t m_client_renderable_addr = 0;
	};
} // namespace sdk