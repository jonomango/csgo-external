#include "datamap_cache.h"

#include "constants.h"

#include <epic/pattern_scanner.h>
#include <crypto/string_encryption.h>


namespace sdk {
	// cache datamap offsets
	void DatamapCache::cache() {
		for (const auto address : mango::find_all_patterns(globals::process, enc_str("client_panorama.dll"),
			enc_str("C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC CC"))) {
			const auto num_fields_addr = globals::process.read<uint32_t>(address + 2);
			const auto num_fields = globals::process.read<uint32_t>(address + 6);
			const auto data_desc_addr = globals::process.read<uint32_t>(address + 12);
			const auto data_desc = globals::process.read<uint32_t>(address + 16);

			// datamap_t::dataDesc is right after datamap_t::dataNumFields in memory
			if (num_fields_addr - data_desc_addr != 4)
				continue;

			// obvious...
			if (num_fields <= 0)
				continue;

			// these should be the same
			if (globals::process.read<uint32_t>(num_fields_addr) != num_fields ||
				globals::process.read<uint32_t>(data_desc_addr) != data_desc)
				continue;

			this->parse_datamap(data_desc_addr);
		}
	}

	// get a field by class and field name hash: fnv1a("class:field")
	uint32_t DatamapCache::get(const uint64_t hash) const {
		if (const auto& it = this->m_fields.find(hash); it != this->m_fields.end())
			return it->second;

		// aw shid
		throw std::runtime_error(enc_str("failed to find datamap field: ") + std::to_string(hash));
	}

	// add each field in the datamap to m_fields
	void DatamapCache::parse_datamap(const uint32_t datamap_addr) {
		// information about a variable basically
		struct TypeDescription {
			uint32_t m_field_type;
			uint32_t m_field_name;
			uint32_t m_field_offset[2];
		private:
			uint8_t m_padding_1[16];
		public:
			uint32_t m_field_size;
		private:
			uint8_t m_padding_2[24];
		};

		// a "map" of variables
		struct Datamap {
			uint32_t m_data_fields; // TypeDescription*
			uint32_t m_num_fields;
			uint32_t m_class_name;
			uint32_t m_base_datamap;
		private:
			uint8_t m_padding_1[6];
		};

		const auto datamap = globals::process.read<Datamap>(datamap_addr);

		// read the classname
		char class_name[256];
		globals::process.read(datamap.m_class_name, class_name, 256);
		class_name[255] = '\0';

		// for each field
		for (size_t i = 0; i < datamap.m_num_fields; ++i) {
			const auto type_desc = globals::process.read<TypeDescription>(datamap.m_data_fields + i * sizeof(TypeDescription));
			if (!type_desc.m_field_name)
				continue;

			// field name
			char field_name[256];
			globals::process.read(type_desc.m_field_name, field_name, 256);
			field_name[255] = '\0';

			const auto hash = mango::fnv1a<uint64_t>((std::string(class_name) + ":" + field_name).c_str());
			this->m_fields[hash] = type_desc.m_field_offset[0];
		}
	}
} // namespace sdk