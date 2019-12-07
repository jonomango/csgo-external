#pragma once

#include <stdint.h>
#include <misc/color.h>
#include <epic/read_write_variable.h>

#include "base_entity.h"
#include "defines.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/glow_outline_effect.h
	class GlowObjectManager {
	public:
		GlowObjectManager() = default;
		GlowObjectManager(const uint32_t address) noexcept;

		// get the underlying address
		operator uint32_t() const noexcept { return this->m_address; }

	public:
		mango::ReadWriteVariable<CUtlVector> m_GlowObjectDefinitions;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk