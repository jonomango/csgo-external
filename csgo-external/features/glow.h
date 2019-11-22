#pragma once

#include "../sdk/classes/base_entity.h"

#include <misc/color.h>


namespace features::glow {
	void draw_entity(const sdk::BaseEntity& entity, const mango::rgbaf& color);
} // namespace features::glow