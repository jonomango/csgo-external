#include "glow.h"

#include "../sdk/misc/constants.h"


namespace features::glow {
	void draw_entity(const sdk::BaseEntity& entity, const mango::rgbaf& color) {
		using namespace sdk;

		// get the address of the glow object
		const auto glow_object_addr = sizeof(GlowObject) * entity.get_glow_index() +
			globals::glow_object_manager.get_glow_object_array();
		auto glow_object = globals::process.read<GlowObject>(glow_object_addr);

		glow_object.m_glow_color = color;
		glow_object.m_render_when_occluded = true;

		// overwrite the glow object with our changed object
		globals::process.write<GlowObject>(glow_object_addr, glow_object);
	}
} // namespace features::glow