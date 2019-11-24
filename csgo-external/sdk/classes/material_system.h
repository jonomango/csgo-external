#pragma once

#include "../misc/interface_base.h"

#include "material.h"

#include <string_view>
#include <optional>


namespace sdk {
	using MaterialHandle = uint16_t;

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h
	class MaterialSystem : public InterfaceBase<MaterialSystem, 
		mango::fnv1a<uint64_t>("materialsystem.dll:VMaterialSystem")> {
	public:
		// get the first valid material handle
		MaterialHandle first_material() const;

		// get the next material
		MaterialHandle next_material(MaterialHandle current) const;

		// get the material address from the handle
		Material get_material(const MaterialHandle handle) const;

		// invalid material handle
		MaterialHandle invalid_material() const { return 0xFFFF; }

		// this just iterates through all materials and compares their names
		Material find_material(const std::string_view name, const std::optional<std::string_view> group_name = {}) const;

	private:
		// each object is like 16 bytes
		uint32_t get_handle_array() const;
	};
} // namespace sdk