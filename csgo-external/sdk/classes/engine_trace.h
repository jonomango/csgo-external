#pragma once

#include "../misc/interface_base.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/engine/IEngineTrace.h
	class EngineTrace : public InterfaceBase<EngineTrace, 
		mango::fnv1a<uint64_t>("engine.dll:EngineTraceClient")>{
	public:

	private:
	};
} // namespace sdk