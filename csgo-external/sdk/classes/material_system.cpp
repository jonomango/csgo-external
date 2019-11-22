#include "material_system.h"

#include "../misc/constants.h"


namespace sdk {
	// get the first valid material handle
	MaterialHandle MaterialSystem::first_material() const {
		const auto handle_array = this->get_handle_array();

		// ref @FirstMaterial
		// not sure byt maybe +0x250 is the last handle in the array and they go up using parents?
		auto current_handle = globals::process.read<MaterialHandle>(this->m_address + 0x250);
		while (current_handle != invalid_material()) {
			const auto next_handle = globals::process.read<MaterialHandle>(handle_array + 16 * current_handle);
			if (next_handle == invalid_material())
				break;

			current_handle = next_handle;
		}

		return current_handle;
	}

	// get the next material
	MaterialHandle MaterialSystem::next_material(MaterialHandle current) const {
		// ref @NextMaterial

		const auto handle_array = this->get_handle_array();

		auto next_handle = current;
		if (next_handle != invalid_material()) {
			// v5 = *(_WORD *)(handleArray + 16 * (unsigned __int16)materialHandle + 2);
			auto v5 = globals::process.read<MaterialHandle>(handle_array + 16 * current + 2);
			if (v5 != invalid_material()) {
				while (v5 != invalid_material()) {
					next_handle = v5;

					// v5 = *(_WORD*)(handleArray + 16 * (unsigned __int16)v5);
					v5 = globals::process.read<MaterialHandle>(handle_array + 16 * v5);
				}

				return next_handle;
			}

			// next_handle = *(_WORD *)(handleArray + 16 * (unsigned __int16)materialHandle + 4);
			next_handle = globals::process.read<MaterialHandle>(handle_array + 16 * current + 4);
		}

		while (true) {
			auto v10 = invalid_material();
			if (current != invalid_material()) {
				// v9 = *(_WORD*)(handleArray_1 + 16 * (unsigned __int16)materialHandle_local + 4)
				const auto v9 = globals::process.read<MaterialHandle>(handle_array + 16 * current + 4);

				if (v9 != invalid_material()) {
					// v10 = *(_WORD *)(handleArray_1 + 16 * v9 + 2);
					v10 = globals::process.read<MaterialHandle>(handle_array + 16 * v9 + 2);
				}
			}

			if (v10 != current)
				break;

			current = next_handle;
			if (next_handle == invalid_material())
				return invalid_material();

			// next_handle = *(_WORD *)(*(_DWORD *)(materialInterface_240h_local + 4) + 16 * (unsigned __int16)next_handle + 4);
			next_handle = globals::process.read<MaterialHandle>(handle_array + 16 * next_handle + 4);
		}

		return next_handle;
	}

	// get the material address from the handle
	Material MaterialSystem::get_material(const MaterialHandle handle) const {
		return Material(globals::process.read<uint32_t>(this->get_handle_array() + uint32_t(handle) * 16 + 8));
	}

	uint32_t MaterialSystem::get_handle_array() const {
		return globals::process.read<uint32_t>(this->m_address + 0x244);
	}
} // namespace sdk