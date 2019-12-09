#include "iclientrenderable.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>

namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientrenderable.h#L86
	uint32_t IClientRenderable::get_model() const {
		// mov eax, [ecx + 0xXX]
		static const auto model_offset = globals::process.read<uint8_t>(mango::get_vfunc<uint32_t>(
			globals::process, this->m_client_renderable_addr, indices::get_model) + 2);
		return globals::process.read<uint32_t>(this->m_client_renderable_addr + model_offset);
	}
} // namespace sdk