#pragma once

#include <epic/process.h>


namespace sdk {
	class NetvarCache {
	public:
		NetvarCache() = default;
		NetvarCache(const mango::Process& process) { this->setup(process); }

		// cache netvars for later use
		void setup(const mango::Process& process);

	private:
	};
} // namespace sdk