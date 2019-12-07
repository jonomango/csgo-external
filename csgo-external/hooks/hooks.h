#pragma once

#include <epic/vmt_hook.h>


namespace hooks {
	inline mango::VmtHook client_mode_vmt_hook;

	// hook stuff and shibble
	void hook();

	// when we're done
	void release();
} // namespace hooks