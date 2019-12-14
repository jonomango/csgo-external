#include "imodelloader.h"

#include "../misc/constants.h"


namespace sdk {
	IModelLoader::IModelLoader(const uint32_t address) noexcept : m_model_loader_addr(address) {
		this->m_Models.setup(globals::process, address + 0x08);
	}

	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L65
	int IModelLoader::get_count() const {
		return this->m_Models().m_iSize;
	}

	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L66
	mango::RWVariable<model_t> IModelLoader::get_model_for_index(const int i) const {
		return { globals::process, globals::process.read<uint32_t>(this->m_Models().m_pData + (i * 16) + 0xC) };
	}

	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/modelloader.h#L78
	mango::RWVariable<model_t> IModelLoader::get_model_for_name(const std::string_view name) const {
		for (int i = 0; i < this->get_count(); ++i) {
			const auto model = this->get_model_for_index(i);
			if (!model)
				continue;

			// simple check for whether the name matches
			if (name == model().szName)
				return model;
		}

		return {};
	}
} // namespace sdk