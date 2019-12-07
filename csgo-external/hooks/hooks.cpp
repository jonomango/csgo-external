#include "hooks.h"

#include "../sdk/misc/constants.h"
#include "../sdk/classes/defines.h"

#include "createmove.h"

#include <thread>


namespace hooks {
	// hook stuff and shibble
	void hook() {
		client_mode_vmt_hook.setup(sdk::globals::process, sdk::globals::client_mode);

		// start hookin!
		hook_createmove();
	}

	// when we're done
	void release() {
		client_mode_vmt_hook.release();

		// sleep before freeing memory
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		// free memory and stuff
		release_createmove();
	}
} // namespace hooks