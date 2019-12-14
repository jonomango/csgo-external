#pragma once

#include "../common.h"

#include <epic/read_write_variable.h>
#include <string_view>


namespace sdk {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L24
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.cpp
	class IModelLoader {
	public:
		IModelLoader() = default;
		explicit IModelLoader(const uint32_t address) noexcept;

		// get the underlying address
		uint32_t get_model_loader_addr() const { return this->m_model_loader_addr; }

	public:
		mango::RWVariable<CUtlMap> m_Models;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L65
		int get_count() const;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L66
		mango::RWVariable<model_t> get_model_for_index(const int i) const;

		// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L78
		mango::RWVariable<model_t> get_model_for_name(const std::string_view name) const;

	private:
		uint32_t m_model_loader_addr = 0;
	};
} // namespace sdk