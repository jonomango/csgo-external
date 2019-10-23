#pragma once

#include <epic/process.h>

#include <string>
#include <unordered_map>


namespace sdk {
	class InterfaceCache {
	public:
		InterfaceCache() = default;
		InterfaceCache(const mango::Process& process) { this->setup(process); }

		// cache all interfaces
		void setup(const mango::Process& process);

		// get an interface without the version at the end: get("ModuleName", "InterfaceName")
		uint32_t get(const std::string& module_name, const std::string& interface_name) const noexcept;

		// get an interface with the version at the end: get("ModuleName", "InterfaceNameXXX")
		uint32_t get_version(const std::string& module_name, const std::string& interface_name) const noexcept;

	private:
		// cache all interfaces for a module
		void cache_interfaces(const mango::Process& process, const std::string& module_name);

	private:
		std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> m_interfaces;
	};
} // namespace sdk