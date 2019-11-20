#pragma once

#include <stdint.h>
#include <misc/color.h>

#include "base_entity.h"


namespace sdk {
	struct GlowObject {
		BaseEntity m_entity;
		mango::rgbaf m_glow_color;
	private:
		uint8_t m_padding_0[0x10];
	public:
		bool m_render_when_occluded;
		bool m_render_when_unoccluded;
	private:
		uint8_t m_padding_1[0xC];
	public:
		int m_next_free_slot;
	}; // sizeof(GlowObject) == 0x38

	class GlowObjectManager {
	public:
		GlowObjectManager() = default;
		GlowObjectManager(const uint32_t address) noexcept;

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		// get num of entries
		int get_size() const;

		// get the array of glow objects
		uint32_t get_glow_object_array() const;

	private:
		uint32_t m_address = 0,
			m_glow_object_definitions = 0;
	};
} // namespace sdk