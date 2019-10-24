#pragma once

#include <stdint.h>

#include "../misc/interface_base.h"


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/client/cdll_client_int.cpp
	class Client : public InterfaceBase<Client,
		mango::fnv1a<uint64_t>("client_panorama.dll:VClient")> {
	public:

		// get the head of the linked list of client classes
		uint32_t get_all_classes() const;
	};
} // namespace sdk