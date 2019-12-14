#include "datamap_cache.h"

#include "constants.h"

#include <epic/pattern_scanner.h>
#include <crypto/string_encryption.h>


namespace sdk {
	// cache datamap offsets
	void DatamapCache::cache(std::optional<std::reference_wrapper<std::ostream>> stream) {
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
			if (num_fields <= 0 || num_fields > 200)
				continue;

			// these should be the same
			if (globals::process.read<uint32_t>(num_fields_addr) != num_fields ||
				globals::process.read<uint32_t>(data_desc_addr) != data_desc)
				continue;
			
			this->parse_datamap(data_desc_addr, stream);
		}
	}

	// get a field by class and field name hash: Fnv1a("class:field")
	uint32_t DatamapCache::get(const uint64_t hash) const {
		if (const auto& it = this->m_fields.find(hash); it != this->m_fields.end())
			return it->second;

		// aw shid
		throw std::runtime_error(enc_str("failed to find datamap field: ") + std::to_string(hash));
	}

	// add each field in the datamap to m_fields
	void DatamapCache::parse_datamap(const uint32_t datamap_addr, std::optional<std::reference_wrapper<std::ostream>> stream) {
		const auto datamap = globals::process.read<Datamap>(datamap_addr);

		// read the classname
		char class_name[256];
		globals::process.read(datamap.m_class_name, class_name, 256);
		class_name[255] = '\0';

		// optional
		if (stream)
			(*stream).get() << class_name << std::endl;

		// for each field
		for (size_t i = 0; i < datamap.m_num_fields; ++i) {
			const auto type_desc = globals::process.read<TypeDescription>(datamap.m_data_fields + i * sizeof(TypeDescription));
			if (!type_desc.m_field_name)
				continue;

			// field name
			char field_name[256];
			globals::process.read(type_desc.m_field_name, field_name, 256);
			field_name[255] = '\0';

			const auto hash = mango::Fnv1a<uint64_t>((std::string(class_name) + ':' + field_name).c_str());
			const auto offset = type_desc.m_field_offset[0];
			this->m_fields[hash] = offset;

			// optional
			if (stream) {
				(*stream).get() << enc_str("    ") << field_name << enc_str(" 0x") << 
					std::hex << std::uppercase << offset << std::endl;
			}
		}
	}
} // namespace sdk