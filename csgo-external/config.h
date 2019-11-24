#pragma once

#include <misc/color.h>


namespace config {
	namespace glow {
		inline bool teammate_enabled = true,
			enemy_enabled = true;
		inline mango::rgbaf teammate_color = { 0.f, 1.f, 1.f, 0.6f },
			enemy_color = { 1.f, 0.f, 0.f, 0.6f };
	} // namespace glow
	namespace misc {
		inline bool radar_enabled = true,
			noflash_enabled = true;
		inline mango::rgbaf nightmode_color = mango::rgbaf(0.05f);
	} // namespace misc
} // namespace config