#include "cglowobjectmanager.h"

#include "../misc/constants.h"


namespace sdk {
	CGlowObjectManager::CGlowObjectManager(const uint32_t address) noexcept : m_address(address) {
		m_GlowObjectDefinitions.setup(globals::process, address);
	}
} // namespace sdk