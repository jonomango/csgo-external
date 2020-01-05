#include "iclientrenderable.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientrenderable.h#L86
	mango::RWVariable<model_t> IClientRenderable::get_model() const {
		// mov eax, [ecx + 0xXX]
		static const auto model_offset = uint32_t(globals::process.read<uint8_t>(mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::get_model) + 2));
		return { globals::process, globals::process.read<uint32_t>(this->m_address + model_offset) };
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/iclientrenderable.h#L147
	mango::RWVariable<ModelInstanceHandle_t> IClientRenderable::get_model_instance() const {
		return { globals::process, this->m_address + 0x2DC };
	}
} // namespace sdk