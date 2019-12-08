#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <ostream>
#include <optional>
#include <functional>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/datamap.h
	class DatamapCache {
	public:
		DatamapCache() = default;

		// cache datamap fields
		void cache(std::optional<std::reference_wrapper<std::ostream>> stream = {});

		// get a field by class and field name hash: fnv1a("class:field")
		uint32_t get(const uint64_t hash) const;

		// forces hash to be compile time constant
		template <uint64_t Hash>
		uint32_t get() const { return this->get(Hash); }

	private:
		// add each field in the datamap to m_fields
		void parse_datamap(const uint32_t datamap_addr, std::optional<
			std::reference_wrapper<std::ostream>> stream);

	private:
		std::unordered_map<uint64_t, uint32_t> m_fields;
	};
} // namespace sdk