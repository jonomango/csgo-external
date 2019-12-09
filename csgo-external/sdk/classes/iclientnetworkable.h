#pragma once

#include <stdint.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientnetworkable.h#L44
	class IClientNetworkable {
	public:
		IClientNetworkable() = default;
		explicit IClientNetworkable(const uint32_t address) noexcept 
			: m_client_networkable_addr(address) {}

		// get the underlying address
		uint32_t get_client_networkable_addr() const { return this->m_client_networkable_addr; }

	public:
		// https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/public/iclientnetworkable.h#L54
		uint32_t get_client_class() const;

		// https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/public/iclientnetworkable.h#L86
		bool is_dormant() const;

		// https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/public/iclientnetworkable.h#L90
		int entindex() const;

	private:
		uint32_t m_client_networkable_addr = 0;
	};
} // namespace sdk