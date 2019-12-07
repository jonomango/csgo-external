#include "glow_object_manager.h"

#include "../misc/constants.h"


namespace sdk {
	GlowObjectManager::GlowObjectManager(const uint32_t address) noexcept : m_address(address) {
		m_GlowObjectDefinitions.setup(globals::process, address);
	}
} // namespace sdk