#include "base_entity.h"

#include "../misc/constants.h"


namespace sdk {
	// get entity health
	int BaseEntity::get_health() const {
		return globals::process.read<int>(this->m_address + offsets::m_iHealth);
	}

	// get entity's flags (like in air or smthn)
	int BaseEntity::get_flags() const {
		return globals::process.read<int>(this->m_address + offsets::m_fFlags);
	}

	// if we can see this entity or not (updates slowly)
	bool BaseEntity::is_spotted() const {
		return globals::process.read<bool>(this->m_address + offsets::m_bSpotted);
	}
	void BaseEntity::set_spotted(const bool value) const {
		return globals::process.write(this->m_address + offsets::m_bSpotted, value);
	}

	// get an entity's team number
	int BaseEntity::get_team() const {
		return globals::process.read<int>(this->m_address + offsets::m_iTeamNum);
	}

	// get an entity's origin position
	mango::Vec3f BaseEntity::get_origin() const {
		return globals::process.read<mango::Vec3f>(this->m_address + offsets::m_vecOrigin);
	}

	// get an entity's networked eye angles
	mango::Vec3f BaseEntity::get_eye_angles() const {
		return globals::process.read<mango::Vec3f>(this->m_address + offsets::m_angEyeAngles);
	}

	// spawn time immunity
	bool BaseEntity::is_immune() const {
		return globals::process.read<bool>(this->m_address + offsets::m_bGunGameImmunity);
	}

	// how long they've been flashed for
	float BaseEntity::get_flash_duration() const {
		return globals::process.read<float>(this->m_address + offsets::m_flFlashDuration);
	}
	void BaseEntity::set_flash_duration(const float value) const {
		globals::process.write(this->m_address + offsets::m_flFlashDuration, value);
	}

	// entity is outside our pvs according to the server
	bool BaseEntity::is_dormant() const {
		return globals::process.read<bool>(this->m_address + offsets::m_bDormant);
	}

	// get the index into the glow object array (called nGlowObjectHandle in the source sdk)
	int BaseEntity::get_glow_index() const {
		return globals::process.read<int>(this->m_address + offsets::m_iGlowIndex);
	}
	// only the z component is nonzero afaik
	mango::Vec3f BaseEntity::get_view_offset() const {
		return globals::process.read<mango::Vec3f>(this->m_address + offsets::m_vecViewOffset);
	}

} // namespace sdk