#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <ostream>
#include <functional>

#include <epic/process.h>
#include <misc/fnv_hash.h>


namespace sdk {
	class NetvarCache {
	public:
		NetvarCache() = default;

		// cache netvars for later use
		void cache(std::optional<std::reference_wrapper<std::ostream>> stream = {});

		// get a netvar by table and prop name hash: Fnv1a("table:prop")
		uint32_t get(const uint64_t hash) const;

		// forces hash to be compile time constant
		template <uint64_t Hash>
		uint32_t get() const { return this->get(Hash); }

	private:
		// recursively parse a RecvTable
		void parse_table(const uint32_t table_address, std::optional<
			std::reference_wrapper<std::ostream>> stream, int indent);

	private:
		std::unordered_map<uint64_t, uint32_t> m_netvars;
	};
} // namespace sdk