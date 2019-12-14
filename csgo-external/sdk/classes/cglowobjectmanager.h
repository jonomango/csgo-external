#pragma once

#include "../common.h"

#include <epic/read_write_variable.h>
#include <misc/color.h>
#include <stdint.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/glow_outline_effect.h#L25
	class CGlowObjectManager {
	public:
		CGlowObjectManager() = default;
		CGlowObjectManager(const uint32_t address) noexcept;

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		mango::RWVariable<CUtlVector<GlowObject>> m_GlowObjectDefinitions;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk