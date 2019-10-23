#pragma once

#include <string>
#include <unordered_map>

#include <epic/process.h>


namespace sdk {
	class NetvarCache {
	public:
		NetvarCache() = default;

		// cache netvars for later use
		void setup();

		// get a netvar by table and prop name
		uint32_t get(const std::string& table, const std::string& prop) const;

	private:
		// recursively parse a RecvTable
		void parse_table(const uint32_t table_address);

	private:
		std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> m_netvars;
	};
} // namespace sdk