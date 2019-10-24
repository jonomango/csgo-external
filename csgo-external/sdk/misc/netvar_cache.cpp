#include "netvar_cache.h"

#include "../misc/constants.h"
#include "../classes/defines.h"

#include <crypto/encrypted_string.h>


namespace sdk {
	// cache netvars for later use
	void NetvarCache::setup() {
		// iterate over all client classes
		auto client_class_addr = interfaces::client.get_all_classes();
		while (client_class_addr) {
			const auto client_class = globals::process.read<ClientClass>(client_class_addr);

			// recursively parse each recv table
			this->parse_table(client_class.m_recv_table);

			// proceed to next node
			client_class_addr = client_class.m_next;
		}
	}

	// get a netvar by table and prop name
	uint32_t NetvarCache::get(const std::string& table, const std::string& prop) const {
		if (const auto& props = this->m_netvars.find(table); props != this->m_netvars.end()) {
			// woohoo we found the netvar
			if (const auto& it = props->second.find(prop); it != props->second.end())
				return it->second;
		}

		// aw shid
		throw std::runtime_error(encrypt_string("failed to find netvar: ") + table + ':' + prop);
	}

	// recursively parse a RecvTable
	void NetvarCache::parse_table(const uint32_t table_address) {
		// sanity check
		if (!table_address)
			return;

		const auto recv_table = globals::process.read<RecvTable>(table_address);

		// read the table name
		char table_name[256];
		globals::process.read(recv_table.m_name, table_name, 255);
		table_name[255] = '\0';

		// iterate over each prop
		for (size_t i = 0; i < recv_table.m_num_props; ++i) {
			const auto prop = globals::process.read<RecvProp>(recv_table.m_props_array + sizeof(RecvProp) * i);

			char prop_name[256];
			globals::process.read(prop.m_name, prop_name, 256);
			prop_name[255] = '\0';

			// frog in a frog
			if (prop.m_type == DPT_DataTable) {
				this->parse_table(prop.m_recv_table);
				continue;
			}

			m_netvars[table_name][prop_name] = prop.m_offset;
		}
	}
} // namespace sdk