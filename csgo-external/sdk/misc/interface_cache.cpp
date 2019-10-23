#include "interface_cache.h"

#include "constants.h"

#include <misc/logger.h>
#include <misc/error_codes.h>

#include <stdint.h>
#include <string>


namespace sdk {
	// cache all interfaces
	void InterfaceCache::setup() {
		// cache interfaces for each module
		for (const auto& [name, _] : globals::process.get_modules()) {
			try {
				this->cache_interfaces(name);
			} catch (mango::FailedToReadMemory&) {}
		}
	}

	// get an interface without the version at the end: get_interface("ModuleName", "InterfaceName")
	uint32_t InterfaceCache::get(const std::string& module_name, const std::string& interface_name) const {
		// get the interfaces for the module
		const auto& interfaces = this->m_interfaces.find(module_name);
		if (interfaces == this->m_interfaces.end())
			throw std::runtime_error("Failed to get interface: " + module_name + ":" + interface_name);

		// look for the interface
		for (const auto& [name, addr] : interfaces->second) {
			// dont compare against the last 3 characters (version)
			if (!name.compare(0, name.size() - 3, interface_name)) {
				mango::logger.success("Interface found: ", module_name, ":", name, ":0x", std::hex, std::uppercase, addr);
				return addr;
			}
		}

		throw std::runtime_error("Failed to get interface: " + module_name + ":" + interface_name);
	}

	// get an interface with the version at the end: get_interface("ModuleName", "InterfaceNameXXX")
	uint32_t InterfaceCache::get_version(const std::string& module_name, const std::string& interface_name) const {
		// get the interfaces for the module
		const auto& interfaces = this->m_interfaces.find(module_name);
		if (interfaces == this->m_interfaces.end())
			throw std::runtime_error("Failed to get interface: " + module_name + ":" + interface_name);

		// look for the interface
		if (const auto& mod = interfaces->second.find(interface_name); mod != interfaces->second.end()) {
			mango::logger.success("Interface found: ", module_name, ":", interface_name, ":0x", std::hex, std::uppercase, mod->second);
			return mod->second;
		}

		throw std::runtime_error("Failed to get interface: " + module_name + ":" + interface_name);
	}

	// cache all interfaces for a module
	void InterfaceCache::cache_interfaces(const std::string& module_name) {
		const auto create_interface_addr = uint32_t(globals::process.get_proc_addr(module_name, "CreateInterface"));
		if (!create_interface_addr)
			return;

		// follow the jmp in CreateInterface to get this (or just CreateInterface - 0x70)
		const auto create_interface_internal_addr = create_interface_addr +
			globals::process.read<uint32_t>(create_interface_addr + 0x5) + 0x9;

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
			char name[128];
			globals::process.read(iface.m_name, name, 128);
			name[127] = '\0';
			
			// follow the function and read the mov operand
			const auto address = globals::process.read<uint32_t>(iface.m_create_fn + 0x1);

			// cache the interface for later
			this->m_interfaces[module_name][name] = address;

			curr_interface_ptr = iface.m_next;
		}
	}
} // namespace sdk