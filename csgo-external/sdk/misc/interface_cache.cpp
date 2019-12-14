#include "interface_cache.h"

#include "constants.h"

#include <crypto/string_encryption.h>

#include <misc/logger.h>
#include <misc/fnv_hash.h>
#include <misc/error_codes.h>

#include <stdint.h>
#include <string>


namespace sdk {
	// cache all interfaces
	void InterfaceCache::cache() {
		// cache interfaces for each module
		for (const auto& [name, _] : globals::process.get_modules())
			this->cache_interfaces(name);
	}

	// get an interface: InterfaceCache::get(InterfaceID(Fnv1a<uint64_t>("ModuleName:InterfaceName"), OptionalVersionNum))
	uint32_t InterfaceCache::get(const InterfaceID interface_id) const {
		// look for the interface
		for (const auto& [id, addr] : this->m_interfaces) {
			if (id.m_hash == interface_id.m_hash)
				return addr;
		}

		// aw shid
		throw std::runtime_error(enc_str("failed to find interface: ") + std::to_string(interface_id.m_hash));
	}

	// cache all interfaces for a module
	void InterfaceCache::cache_interfaces(const std::string& module_name) {
		const auto create_interface_addr = uint32_t(globals::process.get_proc_addr(module_name, enc_str("CreateInterface")));
		if (!create_interface_addr)
			return;

		// 55                      push    ebp
		// 8B EC                   mov     ebp, esp
		// 5D                      pop     ebp
		if (globals::process.read<uint32_t>(create_interface_addr) != 0x5DEC8B55)
			return;

		// jmp create_interface_internal
		const auto create_interface_internal_addr = create_interface_addr + 0x9 +
			globals::process.read<uint32_t>(create_interface_addr + 0x5);

		// linked list of interfaces
		struct InterfaceNode {
			uint32_t m_create_fn;
			uint32_t m_name;
			uint32_t m_next;
		};

		// mov esi, interface_list
		auto curr_interface_ptr = globals::process.read<uint32_t>(
			globals::process.read<uint32_t>(create_interface_internal_addr + 0x6));

		// traverse the linked list of interfaces
		while (curr_interface_ptr) {
			const auto iface = globals::process.read<InterfaceNode>(curr_interface_ptr);

			// read the name
			char buffer[128];
			globals::process.read(iface.m_name, buffer, 128);
			buffer[127] = '\0';
			
			// follow the function and read the mov operand
			const auto address = globals::process.read<uint32_t>(iface.m_create_fn + 0x1);

			// cache the interface for later
			this->m_interfaces[this->make_id(module_name, buffer)] = address;

			curr_interface_ptr = iface.m_next;
		}
	}

	// make an InterfaceID
	InterfaceCache::InterfaceID InterfaceCache::make_id(const std::string& module_name, const std::string& interface_name) const noexcept {
		const auto str = module_name + ":" + interface_name;
		const auto name = str.substr(0, str.size() - 3);
		const auto version = atoi(str.substr(str.size() - 3).c_str());

		return InterfaceID(mango::Fnv1a<uint64_t>(name.c_str()), version);
	}
} // namespace sdk