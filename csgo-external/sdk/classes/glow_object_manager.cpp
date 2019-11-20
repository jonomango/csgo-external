#include "glow_object_manager.h"

#include "../misc/constants.h"


namespace sdk {
	GlowObjectManager::GlowObjectManager(const uint32_t address) noexcept 
		: m_address(address), m_glow_object_definitions(globals::process.read<uint32_t>(address)) {}

	// get num of entries
	int GlowObjectManager::get_size() const {
		return globals::process.read<int>(this->m_address + 0xC);
	}

	// get the array of glow objects
	uint32_t GlowObjectManager::get_glow_object_array() const {
		return this->m_glow_object_definitions;
	}
} // namespace sdk