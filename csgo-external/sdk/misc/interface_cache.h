#pragma once

#include <epic/process.h>
#include <misc/fnv_hash.h>

#include <string>
#include <unordered_map>


namespace sdk {
	class InterfaceCache {
	public:
		struct InterfaceID {
			InterfaceID() = default;
			constexpr InterfaceID(const mango::Fnv1a<uint64_t> hash, const int version = -1) 
				: m_hash(hash), m_version(version) {}

			// for unordered_map
			constexpr bool operator==(const InterfaceID& other) const noexcept {
				return this->m_hash == other.m_hash &&
					this->m_version == other.m_version;
			}

			uint64_t m_hash;
			int m_version;
		};

		struct InterfaceIDHash {
			uint64_t operator()(const sdk::InterfaceCache::InterfaceID& key) const noexcept {
				return key.m_hash ^ (std::hash<uint64_t>()(key.m_version) << 1);
			}
		};

	public:
		InterfaceCache() = default;

		// cache all interfaces
		void cache();

		// get an interface: InterfaceCache::get({ "ModuleName:InterfaceName", OptionalVersionNum })
		uint32_t get(const InterfaceID interface_id) const;

		// forces compile-time hash
		template <uint64_t Hash, int Version = -1>
		uint32_t get() const {
			return this->get({ Hash, Version });
		}

	private:
		// cache all interfaces for a module
		void cache_interfaces(const std::string& module_name);

		// make an InterfaceID
		InterfaceID make_id(const std::string& module_name, const std::string& interface_name) const noexcept;

	private:
		std::unordered_map<InterfaceID, uint32_t, InterfaceIDHash> m_interfaces;
	};
} // namespace sdk