#include "imaterialsystem.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L832
	MaterialHandle_t IMaterialSystem::first_material() const {
		const auto handle_array = this->get_handle_array();

		// not sure but maybe is the last handle in the array and they go up using parents or smthn?
		auto current_handle = globals::process.read<MaterialHandle_t>(this->m_address + this->get_handle_array_offset() + 0x0C);
		while (current_handle != invalid_material()) {
			const auto next_handle = globals::process.read<MaterialHandle_t>(handle_array + 16 * current_handle);
			if (next_handle == invalid_material())
				break;

			current_handle = next_handle;
		}

		return current_handle;
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L837
	MaterialHandle_t IMaterialSystem::next_material(MaterialHandle_t current) const {
		const auto handle_array = this->get_handle_array();

		auto next_handle = current;
		if (next_handle != invalid_material()) {
			// v5 = *(_WORD *)(handleArray + 16 * (unsigned __int16)materialHandle + 2);
			auto v5 = globals::process.read<MaterialHandle_t>(handle_array + 16 * current + 2);
			if (v5 != invalid_material()) {
				while (v5 != invalid_material()) {
					next_handle = v5;

					// v5 = *(_WORD*)(handleArray + 16 * (unsigned __int16)v5);
					v5 = globals::process.read<MaterialHandle_t>(handle_array + 16 * v5);
				}

				return next_handle;
			}

			// next_handle = *(_WORD *)(handleArray + 16 * (unsigned __int16)materialHandle + 4);
			next_handle = globals::process.read<MaterialHandle_t>(handle_array + 16 * current + 4);
		}

		while (true) {
			auto v10 = invalid_material();
			if (current != invalid_material()) {
				// v9 = *(_WORD*)(handleArray_1 + 16 * (unsigned __int16)materialHandle_local + 4)
				const auto v9 = globals::process.read<MaterialHandle_t>(handle_array + 16 * current + 4);

				if (v9 != invalid_material()) {
					// v10 = *(_WORD *)(handleArray_1 + 16 * v9 + 2);
					v10 = globals::process.read<MaterialHandle_t>(handle_array + 16 * v9 + 2);
				}
			}

			if (v10 != current)
				break;

			current = next_handle;
			if (next_handle == invalid_material())
				return invalid_material();

			// next_handle = *(_WORD *)(*(_DWORD *)(materialInterface_240h_local + 4) + 16 * (unsigned __int16)next_handle + 4);
			next_handle = globals::process.read<MaterialHandle_t>(handle_array + 16 * next_handle + 4);
		}

		return next_handle;
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L843
	IMaterial IMaterialSystem::get_material(const MaterialHandle_t handle) const {
		return IMaterial(globals::process.read<uint32_t>(this->get_handle_array() + uint32_t(handle) * 16 + 8));
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L847
	int IMaterialSystem::get_num_materials() const {
		return int(globals::process.read<uint16_t>(this->m_address + this->get_handle_array_offset() + 0x0E));
	}

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L819
	IMaterial IMaterialSystem::find_material(const std::string_view name, const std::optional<std::string_view> group_name) const {
		for (auto handle = this->first_material(); handle != this->invalid_material(); handle = this->next_material(handle)) {
			const auto material = this->get_material(handle);
			if (!material.imaterial())
				continue;

			// this part is optional
			if (group_name && *group_name != material.get_texture_group_name())
				continue;

			// name matches whooo
			if (material.get_name() == name)
				return material;
		}

		return IMaterial(0);
	}

	// each object is like 16 bytes big i think
	uint32_t IMaterialSystem::get_handle_array() const {
		return globals::process.read<uint32_t>(this->m_address + this->get_handle_array_offset());
	}

	// dynamically get the handle array offset
	uint32_t IMaterialSystem::get_handle_array_offset() const {
		static const auto offset = globals::process.read<uint32_t>(mango::get_vfunc<uint32_t>(
			globals::process, this->m_address, indices::get_num_materials) + 3) - 0x0E;
		return offset;
	}
} // namespace sdk