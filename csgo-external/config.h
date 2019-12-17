#pragma once

#include <misc/color.h>
#include <crypto/string_encryption.h>


namespace config {
	// only used to lower compile times
	void setup();

	namespace glow {
		inline bool teammate_enabled, enemy_enabled;
		inline mango::rgbaf teammate_color, enemy_color;
	} // namespace glow
	namespace misc {
		inline bool radar_enabled, noflash_enabled;
		inline mango::rgbaf nightmode_color;
	} // namespace misc
	namespace aimbot {
		inline float accuracy_threshold;
	} // namespace aimbot
	namespace models {
		inline std::string player, knife;
	} // namespace models
} // namespace config