#include "engine_cvar.h"

#include "../misc/constants.h"


namespace sdk {
	// return a pointer to the head of the (linked) list of commands
	uint32_t EngineCvar::get_command_head() const {
		return globals::process.read<uint32_t>(this->m_address + 0x30);
	}

	// get the address of the cvar
	uint32_t EngineCvar::find_var(const std::string_view name) const {
		// iterate over every command until we find a matching one
		for (auto command_addr = this->get_command_head(); command_addr;) {
			const auto command = globals::process.read<ConCommandBase>(command_addr);

			// read the name
			char command_name[128];
			globals::process.read(command.m_name, command_name, sizeof(command_name));
			command_name[127] = '\0';

			// matching name
			if (name == command_name)
				return command_addr;

			// proceed to next one
			command_addr = command.m_next;
		}

		return 0;
	}
} // namespace sdk