#pragma once

#include <stdint.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/IEngineTrace.h#L128
	class IEngineTrace {
	public:
		IEngineTrace() noexcept : m_address(0) {}
		explicit IEngineTrace(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		uint32_t ienginetrace() const noexcept { return this->m_address; }

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk