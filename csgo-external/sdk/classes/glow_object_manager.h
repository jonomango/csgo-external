#pragma once

#include <stdint.h>
#include <misc/color.h>

#include "base_entity.h"
#include "defines.h"


namespace sdk {
	class GlowObjectManager {
	public:
		GlowObjectManager() = default;
		GlowObjectManager(const uint32_t address) noexcept;

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		// on map change
		void update_object_definitions();

		// get num of entries
		int get_size() const;

		// get the array of glow objects
		uint32_t get_glow_object_array() const;

	private:
		uint32_t m_address = 0,
			m_glow_object_definitions = 0;
	};
} // namespace sdk