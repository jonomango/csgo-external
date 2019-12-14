#pragma once

#include "../sdk/classes/c_csplayer.h"

#include <string_view>


namespace features::modelchanger {
	// player model changer
	void update_player(const std::string_view modelname);

	// knife model changer
	void update_knife(const std::string_view modelname);
} // namespace features::modelchanger