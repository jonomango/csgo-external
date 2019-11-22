#pragma once

#include <stdint.h>
#include <string>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterial.h
	class Material {
	public:
		Material() = default;
		explicit Material(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		// material name
		std::string get_name() const;

		// material texture group name
		std::string get_texture_group_name() const;

		// set the alpha of the material
		void alpha_modulate(const float value) const;

		// an array of IMaterialVar*
		uint32_t get_shader_params() const;

	private:
		// i don't fully understand their mechanism for this, but basically a call to GetName()
		// or GetTextureGroupName() just adds a constant value to ecx (thisptr) then calls a function
		// which will then get the name/texture group name. weird.
		std::string get_name_imp(const uint32_t offset) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk