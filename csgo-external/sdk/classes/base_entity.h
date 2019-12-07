#pragma once

#include <stdint.h>
#include <misc/vector.h>
#include <misc/matrix.h>
#include <epic/read_write_variable.h>

#include "defines.h"
#include "studio.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/client/c_baseentity.h#L174
	// the inheritence is not accurate, most of this stuff should be spread 
	// out into C_BasePlayer, C_CSPlayer, C_BaseAnimating, DT_BaseCombatCharacter, ...
	class BaseEntity {
	public:
		BaseEntity() = default;
		explicit BaseEntity(const uint32_t address) noexcept;

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		mango::ReadWriteVariable<int> m_iHealth;
		mango::ReadWriteVariable<int> m_fFlags;
		mango::ReadWriteVariable<bool> m_bSpotted;
		mango::ReadWriteVariable<int> m_iTeamNum;
		mango::ReadWriteVariable<mango::Vec3f> m_vecOrigin;
		mango::ReadWriteVariable<mango::Vec3f> m_angEyeAngles;
		mango::ReadWriteVariable<bool> m_bGunGameImmunity;
		mango::ReadWriteVariable<float> m_flFlashDuration;
		mango::ReadWriteVariable<int> m_nTickBase;
		mango::ReadWriteVariable<uint32_t> m_hActiveWeapon;
		mango::ReadWriteVariable<bool> m_bDormant;
		mango::ReadWriteVariable<int> m_iGlowIndex;
		mango::ReadWriteVariable<mango::Vec3f> m_vecViewOffset;
		mango::ReadWriteVariable<mango::Vec3f> m_aimPunchAngle;
		mango::ReadWriteVariable<BoneAccessor> m_BoneAccessor;

		// get the matrix for a bone
		mango::Matrix3x4f get_bone_matrix(const int i) const;

		// get the IClientRenderable
		uint32_t get_client_renderable() const;

		// get the model_t*
		uint32_t get_model() const;

		// get a hitbox duh
		mstudiobbox_t get_hitbox(const int index) const;

	protected:
		uint32_t m_address = 0;
	};
} // namespace sdk