#include "hooks.h"

#include "createmove.h"
#include "../sdk/misc/constants.h"

#include <thread>




#include <epic/shellcode.h>


namespace hooks {
	// hook stuff and shibble
	void hook() {
		client_mode_vmt_hook.setup(sdk::globals::process, sdk::globals::client_mode.iclientmode());

		// start hookin!
		hook_createmove();

		static constexpr auto ShouldDrawParticles = 16;
		static constexpr auto ShouldDrawFog = 17;
		static constexpr auto ShouldDrawViewModel = 27;
		static constexpr auto ShouldDrawCrosshair = 28;
		static constexpr auto ShouldDrawCrosshair = 28;
		static constexpr auto GetViewModelFOV = 35;
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