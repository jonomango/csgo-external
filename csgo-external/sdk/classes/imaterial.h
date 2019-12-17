#pragma once

#include "../common.h"

#include <misc/color.h>
#include <stdint.h>
#include <string>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L446
	class IMaterial {
	public:
		IMaterial() = default;
		explicit IMaterial(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		uint32_t imaterial() const noexcept { return this->m_address; }

	public:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L452
		std::string get_name() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L453
		std::string get_texture_group_name() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L531
		void alpha_modulate(const float value) const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L532
		void color_modulate(const mango::rgbaf& value) const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h#L569
		uint32_t get_shader_params() const;

	private:
		// i don't fully understand their mechanism for this, but basically a call to GetName()
		// or GetTextureGroupName() just adds a constant value to ecx (thisptr) then calls a function
		// which will then get the name/texture group name. maybe (this + OFFSET) represents a handle
		// for the string which is then used to get the name. weird.
		std::string get_name_imp(const uint32_t offset) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk