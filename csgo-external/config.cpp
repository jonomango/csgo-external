#include "config.h"


namespace config {
	// only used to lower compile times
	void setup() {
		// teammates
		glow::teammate_color = { 0.f, 1.f, 1.f, 0.6f };
		glow::teammate_enabled = true;

		// enemies
		glow::enemy_color = { 1.f, 0.f, 0.f, 0.6f };
		glow::enemy_enabled = true;

		// misc
		misc::radar_enabled = true;
		misc::noflash_enabled = true;
		misc::nightmode_color = mango::rgbaf(0.05f);

		// aimbot
		aimbot::accuracy_threshold = 0.016f;

		// models
		models::player = enc_str("models/player/custom_player/legacy/ctm_st6_variante.mdl");
		models::knife = enc_str("models/weapons/v_knife_bayonet.mdl");
	}
} // namespace config