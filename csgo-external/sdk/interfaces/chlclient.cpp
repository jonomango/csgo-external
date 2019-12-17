#include "chlclient.h"

#include "../misc/constants.h"

#include <epic/vmt_helpers.h>


namespace sdk {
	// get the head of the linked list of client classes
	mango::RWVariable<ClientClass> CHLClient::get_all_classes() const {
		// the head node to a linked list of ClientClass pointers
		static const auto client_class_head = globals::process.read<uint32_t>(mango::get_vfunc<uint32_t
			>(globals::process, interfaces::client.chlclient(), indices::get_all_classes) + 1);
		return { globals::process, globals::process.read<uint32_t>(client_class_head) };
	}
} // namespace sdk