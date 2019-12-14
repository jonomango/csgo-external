#include "glow.h"

#include "../sdk/misc/constants.h"


namespace features::glow {
	void draw_player(const sdk::C_CSPlayer& player, const mango::rgbaf& color) {
		using namespace sdk;

		// get the address of the glow object
		const auto glow_object = globals::glow_object_manager.m_GlowObjectDefinitions()[player.m_iGlowIndex()];

		auto new_glow_object = glow_object();
		new_glow_object.m_render_when_occluded = true;
		new_glow_object.m_glow_color = color;

		// overwrite the glow object with our changed object
		glow_object = new_glow_object;
	}
} // namespace features::glow