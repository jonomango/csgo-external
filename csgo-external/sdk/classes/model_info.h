#pragma once

#include "../misc/interface_base.h"


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/engine/ivmodelinfo.h
	class ModelInfo : public InterfaceBase<ModelInfo,
		mango::fnv1a<uint64_t>("engine.dll:VModelInfoClient")> {
	public:
		// get the studiohdr_t* for a model_t*
		uint32_t get_studio_hdr(const uint32_t modelptr) const;
	};
} // namespace sdk