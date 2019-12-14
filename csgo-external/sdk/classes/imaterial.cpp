#include "imaterial.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>
#include <crypto/string_encryption.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L452
	std::string IMaterial::get_name() const {
		const auto vfunc = mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::get_name);

		// add ecx, 0xXX
		return this->get_name_imp(globals::process.read<uint8_t>(vfunc + 0x02));
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L453
	std::string IMaterial::get_texture_group_name() const {
		const auto vfunc = mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::get_texture_group_name);

		// add ecx, 0xXX
		return this->get_name_imp(globals::process.read<uint8_t>(vfunc + 0x02));
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L531
	void IMaterial::alpha_modulate(const float value) const {
		const auto shader_params = this->get_shader_params();
		if (!shader_params)
			return;

		const auto var_addr = globals::process.read<uint32_t>(shader_params + 0x14);
		if (!var_addr)
			return;
		
		globals::process.write(var_addr + offsetof(MaterialVar, m_vec_value), value);
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L532
	void IMaterial::color_modulate(const mango::rgbaf& value) const {
		const auto shader_params = this->get_shader_params();
		if (!shader_params)
			return;

		const auto var_addr = globals::process.read<uint32_t>(shader_params + 0x10);
		if (!var_addr)
			return;

		globals::process.write(var_addr + offsetof(MaterialVar, m_vec_value), value);
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L569
	uint32_t IMaterial::get_shader_params() const {
		// read the first 2 bytes of the vfunc
		const auto func_start = globals::process.read<uint16_t>(mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::get_shader_params));

		// ghetto way to check which derived class is calling this function
		if (func_start == 0x418B) {
			// mov eax, [ecx + 24h]
			return globals::process.read<uint32_t>(this->m_address + 0x24);
		} else if (func_start == 0x498B) {
			// mov ecx, [ecx + 4h]
			return IMaterial(globals::process.read<uint32_t>(this->m_address + 0x04)).get_shader_params();
		}

		throw std::runtime_error(enc_str("Failed to get shader params."));
	}

	// i don't fully understand their mechanism for this, but basically a call to GetName()
	// or GetTextureGroupName() just adds a constant value to ecx (thisptr) then calls a function
	// which will then get the name/texture group name. maybe (this + OFFSET) represents a handle
	// for the string which is then used to get the name. weird.
	std::string IMaterial::get_name_imp(const uint32_t offset) const {
		// mov     eax, someInstance
		// mov     ecx, eax
		// call    getName
		auto eax = uint32_t(globals::process.read<uint16_t>(this->m_address + offset));

		// has never happened for me
		if (eax == 0xFFFF)
			return "";

		// movzx   eax, ax
		// lea     ecx, [eax + eax * 2]
		// mov     eax, [edi + 4]
		// movzx   edx, word ptr[eax + ecx * 4 + 8]
		// lea     ecx, [eax + ecx * 4]
		// mov     eax, [edi + 24h]
		// movzx   ecx, word ptr[ecx + 0Ah]
		// mov     eax, [eax + edx * 4]
		// add     eax, 0Ah
		// add     eax, ecx

		auto edi = globals::material_name_related_var;
		auto ecx = eax * 3;
		eax = globals::process.read<uint32_t>(edi + 4);
		auto edx = uint32_t(globals::process.read<uint16_t>(eax + ecx * 4 + 8));
		ecx = eax + ecx * 4;
		eax = globals::process.read<uint32_t>(edi + 0x24);
		ecx = uint32_t(globals::process.read<uint16_t>(ecx + 0xA));
		eax = globals::process.read<uint32_t>(eax + edx * 4) + 0xA + ecx;

		// result is stored in eax
		char name_buffer[256];
		globals::process.read(eax, name_buffer, 256);
		name_buffer[255] = '\0';

		return name_buffer;
	}
} // namespace sdk