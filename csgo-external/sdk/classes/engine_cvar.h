#pragma once

#include "../misc/interface_base.h"

#include "defines.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icvar.h
	class EngineCvar : public InterfaceBase<EngineCvar,
		mango::fnv1a<uint64_t>("vstdlib.dll:VEngineCvar")> {
	public:
		
		// return a pointer to the head of the (linked) list of commands
		uint32_t get_command_head() const;

		// get the address of the cvar
		uint32_t find_var(const std::string_view name) const;
	};
} // namespace sdk