#include "base_entity.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>


namespace sdk {
	BaseEntity::BaseEntity(const uint32_t address) noexcept : m_address(address) {
		this->m_iHealth.setup(globals::process, address + offsets::m_iHealth);
		this->m_fFlags.setup(globals::process, address + offsets::m_fFlags);
		this->m_bSpotted.setup(globals::process, address + offsets::m_bSpotted);
		this->m_iTeamNum.setup(globals::process, address + offsets::m_iTeamNum);
		this->m_vecOrigin.setup(globals::process, address + offsets::m_vecOrigin);
		this->m_angEyeAngles.setup(globals::process, address + offsets::m_angEyeAngles);
		this->m_bGunGameImmunity.setup(globals::process, address + offsets::m_bGunGameImmunity);
		this->m_flFlashDuration.setup(globals::process, address + offsets::m_flFlashDuration);
		this->m_nTickBase.setup(globals::process, address + offsets::m_nTickBase);
		this->m_hActiveWeapon.setup(globals::process, address + offsets::m_hActiveWeapon);
		this->m_bDormant.setup(globals::process, address + offsets::m_bDormant);
		this->m_iGlowIndex.setup(globals::process, address + offsets::m_iGlowIndex);
		this->m_vecViewOffset.setup(globals::process, address + offsets::m_vecViewOffset);
		this->m_aimPunchAngle.setup(globals::process, address + offsets::m_aimPunchAngle);
		this->m_BoneAccessor.setup(globals::process, address + offsets::m_BoneAccessor);
	}

	// get the matrix for a bone
	mango::Matrix3x4f BaseEntity::get_bone_matrix(const int i) const {
		return globals::process.read<mango::Matrix3x4f>(
			sizeof(mango::Matrix3x4f) * i + this->m_BoneAccessor().m_pBones);
	}

	// get the IClientRenderable
	uint32_t BaseEntity::get_client_renderable() const {
		return this->m_address + 0x04;
	}

	// get the model_t*
	uint32_t BaseEntity::get_model() const {
		const auto client_renderable = this->get_client_renderable();

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseentity.h#L1295
		// mov eax, [ecx + model_offset]
		static const auto model_offset = globals::process.read<uint8_t>(
			mango::get_vfunc<uint32_t>(globals::process, client_renderable, indices::get_model) + 0x02);

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