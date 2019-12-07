#pragma once


namespace hooks {
	// allocate and write shellcode then hook
	void hook_createmove();

	// free memory and cleanup
	void release_createmove();
} // namespace hooks