#include "chlclient.h"

#include "../misc/constants.h"


namespace sdk {
	// get the head of the linked list of client classes
	uint32_t CHLClient::get_all_classes() const {
		// ref @GetAllClasses
		return globals::process.read<uint32_t>(globals::client_class_head);
	}
} // namespace sdk