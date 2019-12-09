#pragma once

#include "../sdk/classes/c_csplayer.h"

#include <misc/color.h>


namespace features::glow {
	void draw_player(const sdk::C_CSPlayer& player, const mango::rgbaf& color);
} // namespace features::glow