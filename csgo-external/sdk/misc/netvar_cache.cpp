#include "netvar_cache.h"

#include "../misc/constants.h"
#include "../common.h"

#include <crypto/string_encryption.h>


namespace sdk {
	// cache netvars for later use
	void NetvarCache::cache(std::optional<std::reference_wrapper<std::ostream>> stream) {
		// iterate over all client classes
		auto client_class_addr = &interfaces::client.get_all_classes();
		while (client_class_addr) {
			const auto client_class = globals::process.read<ClientClass>(client_class_addr);

			// recursively parse each recv table
			this->parse_table(client_class.m_recv_table, stream, 0);

			// proceed to next node
			client_class_addr = client_class.m_next;
		}
	}

	// get a netvar by table and prop name hash: Fnv1a("table:prop")
	uint32_t NetvarCache::get(const uint64_t hash) const {
		if (const auto& it = this->m_netvars.find(hash); it != this->m_netvars.end())
			return it->second;

		// aw shid
		throw std::runtime_error(enc_str("failed to find netvar: ") + std::to_string(hash));
	}

	// recursively parse a RecvTable
	void NetvarCache::parse_table(const uint32_t table_address, std::optional<std::reference_wrapper<std::ostream>> stream, int indent) {
		// sanity check
		if (!table_address)
			return;

		const auto recv_table = globals::process.read<RecvTable>(table_address);

		// read the table name
		char table_name[256];
		globals::process.read(recv_table.m_name, table_name, 255);
		table_name[255] = '\0';

		// optional
		if (stream) {
			(*stream).get() << std::string(indent, ' ') << table_name << std::endl;
			indent += 4;
		}

		// iterate over each prop
		for (size_t i = 0; i < recv_table.m_num_props; ++i) {
			const auto prop = globals::process.read<RecvProp>(recv_table.m_props_array + sizeof(RecvProp) * i);

			// frog in a frog
			if (prop.m_type == DPT_DataTable)
				this->parse_table(prop.m_recv_table, stream, indent);

			char prop_name[256];
			globals::process.read(prop.m_name, prop_name, 256);
			prop_name[255] = '\0';

			const auto hash = mango::Fnv1a<uint64_t>((std::string(table_name) + ":" + prop_name).c_str());
			this->m_netvars[hash] = prop.m_offset;

			// optional
			if (stream) {
				(*stream).get() << std::string(indent, ' ') << prop_name << enc_str(" 0x") <<
					std::hex << std::uppercase << prop.m_offset << std::endl;
			}
		}
	}
} // namespace sdk