#include "nightmode.h"

#include <crypto/string_encryption.h>

#include "../sdk/misc/constants.h"


namespace features::nightmode {
	void modulate(const mango::rgbaf& color) {
		using namespace sdk;

		// material modulation
		for (auto handle = interfaces::material_system.first_material();
			handle != interfaces::material_system.invalid_material();
			handle = interfaces::material_system.next_material(handle)) {

			const auto material = interfaces::material_system.get_material(handle);

			// modulate the world
			if (material.get_texture_group_name() == enc_str("World textures"))
				material.color_modulate(color);
		}
	}
} // namespace features::nightmode