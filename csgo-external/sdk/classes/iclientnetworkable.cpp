#include "iclientnetworkable.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>


namespace sdk {
	// https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/public/iclientnetworkable.h#L54
	uint32_t IClientNetworkable::get_client_class() const {
		// mov eax, 0xXXXXXXXX
		const auto vfunc = mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::get_client_class);
		return globals::process.read<uint32_t>(vfunc + 1);
 	}

	// https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/public/iclientnetworkable.h#L86
	bool IClientNetworkable::is_dormant() const {
		// based off of the C_BaseEntity override of IsDormant()

		// entity not known to the server
		if (this->entindex() == -1)
			return false;

		// mov al, [ecx + 0xXXXXXXXX]
		static const auto offset = globals::process.read<uint8_t>(mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::is_dormant) + 8);
		return globals::process.read<bool>(this->m_address + offset);
	}

	// https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/public/iclientnetworkable.h#L90
	int IClientNetworkable::entindex() const {
		// mov eax, [ecx + 0xXX]
		static const auto offset = globals::process.read<uint8_t>(mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::entindex) + 2);
		return globals::process.read<uint8_t>(this->m_address + offset);
	}
} // namespace sdk