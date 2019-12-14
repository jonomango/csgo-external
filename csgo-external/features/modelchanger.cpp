#include "modelchanger.h"

#include "../sdk/classes/c_weaponcsbase.h"
#include "../sdk/misc/constants.h"

#include <crypto/string_encryption.h>


namespace features::modelchanger {
	// player model changer
	void update_player(const std::string_view modelname) {
		using namespace sdk;

		// get localplayer
		const auto localplayer = interfaces::client_entity_list.get_local_player();

		// model changer
		if (const auto new_model = globals::model_loader.get_model_for_name(modelname)) {
			// override our model's mdl_handle with our desired model's mdl_handle
			globals::process.write<MDLHandle_t>(&localplayer.get_model() +
				offsetof(model_t, studio), new_model().studio);
		}
	}

	// knife model changer
	void update_knife(const std::string_view modelname) {
		using namespace sdk;

		// get localplayer
		const auto localplayer = interfaces::client_entity_list.get_local_player();
		if (!localplayer.get_csplayer_addr())
			return;

		// find our knife model
		const auto weapons = localplayer.m_hMyWeapons();
		for (size_t i = 0; i < 64; ++i) {
			const auto handle = weapons[i];
			if (handle == 0xFFFF'FFFF)
				break;

			const auto weapon = interfaces::client_entity_list.get_client_entity<
				C_WeaponCSBase>(handle & 0xFFF);
			const auto model = weapon.get_model();
			if (!model)
				continue;

			// we only care about the knife
			if (std::string(model().szName).find(enc_str("knife")) == std::string::npos)
				continue;

			// our new knife
			const auto new_model = globals::model_loader.get_model_for_name(modelname);
			if (!new_model)
				break;

			// override our model's mdl_handle with our desired model's mdl handle
			globals::process.write<MDLHandle_t>(uint32_t(&model) +
				offsetof(model_t, studio), new_model().studio);

			// m_iViewModelIndex
			//globals::process.write<uint32_t>(weapon.get_client_renderable_addr() + 0x68, 0);
			//mango::logger.info(globals::process.read<int>(weapon.get_weaponcs_base_addr() + 0x29C4));
			//globals::process.write<int>(weapon.get_weaponcs_base_addr() + 0x3220, 477);

			// you can't have two knives dummy
			break;
		}
	}
} // namespace features::modelchanger