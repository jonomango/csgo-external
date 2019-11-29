#pragma once

#include <stdint.h>
#include <misc/vector.h>


namespace sdk {
	// not really accurate, most of this stuff should be spread out into C_BasePlayer, C_CSPlayer, C_BaseAnimating, DT_BaseCombatCharacter, ...
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/c_baseentity.h#L174
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
		void set_spotted(const bool value) const;

		// get an entity's team number
		int get_team() const;

		// get an entity's origin position
		mango::Vec3f get_origin() const;

		// get an entity's networked eye angles
		mango::Vec3f get_eye_angles() const;

		// spawn time immunity
		bool is_immune() const;

		// how long they've been flashed for
		float get_flash_duration() const;
		void set_flash_duration(const float value) const;

		// entity is outside our pvs according to the server
		bool is_dormant() const;

		// get the index into the glow object array (called nGlowObjectHandle in the source sdk)
		int get_glow_index() const;

		// only the z component is nonzero afaik
		mango::Vec3f get_view_offset() const;

		// only applies to localplayer
		int get_tick_base() const;

		// this returns the active weapon INDEX
		int get_active_weapon() const;

		// recoil
		mango::Vec3f get_aim_punch() const;

	protected:
		uint32_t m_address = 0;
	};
} // namespace sdk