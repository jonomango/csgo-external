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

	// only applies to localplayer
	int BaseEntity::get_tick_base() const {
		return globals::process.read<int>(this->m_address + offsets::m_nTickBase);
	}

	// this returns the active weapon INDEX
	int BaseEntity::get_active_weapon() const {
		return globals::process.read<int>(this->m_address + offsets::m_hActiveWeapon) & 0xFFF;
	}

	// recoil
	mango::Vec3f BaseEntity::get_aim_punch() const {
		return globals::process.read<mango::Vec3f>(this->m_address + offsets::m_Local + offsets::m_aimPunchAngle);
	}

	// get the bone accessor
	BoneAccessor BaseEntity::get_bone_accessor() const {
		return globals::process.read<BoneAccessor>(this->m_address + offsets::m_BoneAccessor);
	}

	// get the matrix for a bone
	mango::Matrix3x4f BaseEntity::get_bone_matrix(const int i) const {
		return globals::process.read<mango::Matrix3x4f>(
			this->get_bone_accessor().m_pBones + sizeof(mango::Matrix3x4f) * i);
	}

	// get the IClientRenderable
	uint32_t BaseEntity::get_client_renderable() const {
		return this->m_address + 0x04;
	}

	// get the model_t*
	uint32_t BaseEntity::get_model() const {
		const auto client_renderable = this->get_client_renderable();

		// mov eax, [ecx + model_offset]
		static const auto model_offset = globals::process.read<uint8_t>(
			globals::process.get_vfunc<uint32_t>(client_renderable, indices::get_model) + 0x02);

		return globals::process.read<uint32_t>(client_renderable + model_offset);
	}

	// get a hitbox duh
	mstudiobbox_t BaseEntity::get_hitbox(const int index) const {
		// studiohdr
		const auto studiohdr_addr = interfaces::model_info.get_studio_hdr(this->get_model());
		const auto studiohdr = globals::process.read<studiohdr_t>(studiohdr_addr);

		// hitboxset
		const auto hitboxset_addr = studiohdr.get_hitbox_set(studiohdr_addr, 0);
		const auto hitboxset = globals::process.read<mstudiohitboxset_t>(hitboxset_addr);

		// hitbox
		const auto hitbox_addr = hitboxset.get_hitbox(hitboxset_addr, 0);
		return globals::process.read<mstudiobbox_t>(hitbox_addr);
	}
} // namespace sdk