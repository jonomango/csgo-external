#pragma once

#include "icliententity.h"
#include "studio.h"
#include "../common.h"

#include <stdint.h>
#include <misc/vector.h>
#include <misc/matrix.h>
#include <epic/read_write_variable.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseentity.h#L175
	class C_BaseEntity : public IClientEntity {
	public:
		C_BaseEntity() = default;
		explicit C_BaseEntity(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t cbaseentity() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<bool> m_bSpotted;
		mango::RWVariable<int> m_iTeamNum;
		mango::RWVariable<int> m_fEffects;
		mango::RWVariable<mango::Vec3f> m_vecOrigin;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/c_baseentity.h#L1049
		void set_model_pointer(const mango::RWVariable<model_t> new_model) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk