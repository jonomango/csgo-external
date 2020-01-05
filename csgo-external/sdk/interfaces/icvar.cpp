#include "icvar.h"

#include "../misc/constants.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icvar.h#L79
	uint32_t ICvar::get_commands() const {
		return globals::process.read<uint32_t>(this->m_address + 0x30);
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icvar.h#L75
	uint32_t ICvar::find_command(const std::string_view name) const {
		// iterate over every command until we find a matching one
		for (auto command_addr = this->get_commands(); command_addr;) {
			const auto command = globals::process.read<ConCommandBase>(command_addr);

			// read the name
			char command_name[128];
			globals::process.read(command.m_pszName, command_name, sizeof(command_name));
			command_name[127] = '\0';

			// matching name
			if (name == command_name)
				return command_addr;

			// proceed to next one
			command_addr = command.m_pNext;
		}

		return 0;
	}
} // namespace sdk