#pragma once

#include "../common.h"

#include <stdint.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icvar.h#L55
	class ICvar {
	public:
		ICvar() noexcept : m_address(0) {}
		explicit ICvar(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		constexpr operator uint32_t() const noexcept { return this->m_address; }

	public:	
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icvar.h#L79
		uint32_t get_commands() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icvar.h#L75
		uint32_t find_command(const std::string_view name) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk