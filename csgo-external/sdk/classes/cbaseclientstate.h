#pragma once

#include <epic/read_write_variable.h>
#include <misc/vector.h>


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/baseclientstate.h#L66
	class CBaseClientState {
	public:
		CBaseClientState() = default;
		explicit CBaseClientState(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t base_client_state() const { return this->m_base_client_state; }

	public:
		mango::RWVariable<int> m_nSignonState;
		mango::RWVariable<int> m_nDeltaTick;
		mango::RWVariable<int> m_nPlayerSlot;
		mango::RWVariable<mango::Vec3f> m_vecViewAngles;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/baseclientstate.h#L150
		void force_full_update() const;

	private:
		uint32_t m_base_client_state = 0;
	};
} // namespace sdk