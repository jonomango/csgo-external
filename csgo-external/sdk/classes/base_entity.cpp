#include "base_entity.h"

#include "../misc/constants.h"


namespace sdk {
	// get entity health
	int BaseEntity::get_health() const {
		return globals::process.read<int>(this->m_address + netvars::m_iHealth);
	}

	// get entity's flags (like in air or smthn)
	int BaseEntity::get_flags() const {
		return globals::process.read<int>(this->m_address + netvars::m_fFlags);
	}

	// if we can see this entity or not (updates slowly)
	bool BaseEntity::is_spotted() const {
		return globals::process.read<bool>(this->m_address + netvars::m_bSpotted);
	}

	// get an entity's team number
	int BaseEntity::get_team() const {
		return globals::process.read<int>(this->m_address + netvars::m_iTeamNum);
	}

	// get an entity's origin position
	mango::vec3f BaseEntity::get_origin() const {
		return globals::process.read<mango::vec3f>(this->m_address + netvars::m_vecOrigin);
	}

	// get an entity's networked eye angles
	mango::vec3f BaseEntity::get_eye_angles() const {
		return globals::process.read<mango::vec3f>(this->m_address + netvars::m_angEyeAngles);
	}

	// spawn time immunity
	bool BaseEntity::is_immune() const {
		return globals::process.read<bool>(this->m_address + netvars::m_bGunGameImmunity);
	}

	// how long they've been flashed for
	float BaseEntity::get_flash_duration() const {
		return globals::process.read<float>(this->m_address + netvars::m_flFlashDuration);
	}
} // namespace sdk