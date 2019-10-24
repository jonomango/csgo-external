#pragma once

#include <stdint.h>

#include <misc/vector.h>


namespace sdk {
	class BaseEntity {
	public:
		BaseEntity() = default;
		explicit BaseEntity(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		// get entity health
		int get_health() const;

		// get entity's flags (like in air or smthn)
		int get_flags() const;

		// if we can see this entity or not (updates slowly)
		bool is_spotted() const;

		// get an entity's team number
		int get_team() const;

		// get an entity's origin position
		mango::vec3f get_origin() const;

		// get an entity's networked eye angles
		mango::vec3f get_eye_angles() const;

		// spawn time immunity
		bool is_immune() const;

		// how long they've been flashed for
		float get_flash_duration() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk