#include <epic/process.h>
#include <epic/shellcode.h>
#include <epic/pattern_scanner.h>
#include <misc/logger.h>
#include <misc/vector.h>
#include <misc/matrix.h>
#include <misc/fnv_hash.h>
#include <misc/scope_guard.h>
#include <misc/error_codes.h>
#include <crypto/string_encryption.h>

#include "sdk/misc/constants.h"
#include "sdk/classes/c_weaponcsbase.h"
#include "sdk/classes/c_csplayer.h"
#include "sdk/classes/studio.h"

#include "hooks/hooks.h"
#include "features/glow.h"
#include "features/nightmode.h"
#include "features/modelchanger.h"

#include "config.h"

#include <thread>
#include <sstream>


// setup logger channels
void setup_logger() {
	static const auto display_info = [](const uint16_t attribute, const std::string_view prefix, std::ostringstream&& ss) {
		static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

		std::cout << '[';
		SetConsoleTextAttribute(handle, attribute);
		std::cout << prefix;
		SetConsoleTextAttribute(handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		std::cout << "] " << ss.str() << std::endl;
	};

	// info channel
	mango::logger.set_info_channel([](std::ostringstream&& ss) {
		display_info(FOREGROUND_BLUE | FOREGROUND_GREEN, enc_str("info"), std::move(ss));
	});

	// success channel
	mango::logger.set_success_channel([](std::ostringstream&& ss) {
		display_info(FOREGROUND_GREEN, enc_str("success"), std::move(ss));
	});

	// error channel
	mango::logger.set_error_channel([](std::ostringstream&& ss) {
		display_info(FOREGROUND_RED, enc_str("error"), std::move(ss));
	});

	mango::logger.success(enc_str("Logging channels initialized."));
}

// sets up stuff (interfaces, netvars, ...)
void setup_cheat() {
	const auto pids = mango::Process::get_pids_by_name(enc_str("csgo.exe"));
	if (pids.empty()) {
		throw std::runtime_error(enc_str("csgo is not open."));
	} else if (pids.size() > 1) {
		throw std::runtime_error(enc_str("Multiple csgo's open."));
	}

	mango::logger.success(enc_str("Found Process ID: 0x"), std::hex, std::uppercase, pids.front());

	// options for setting up the process
	mango::Process::SetupOptions options;
	options.m_defer_module_loading = false;

	// setup process
	mango::logger.info(enc_str("Setting up process..."));
	sdk::globals::process.setup(pids.front(), options);
	mango::logger.success(enc_str("Process initialized: "), sdk::globals::process.get_name());

	// gets interfaces and stuffs and shibs
	sdk::setup_constants();

	// setup our hooks
	hooks::hook();
}

// cleanup
void release_cheat() {
	hooks::release();

	sdk::globals::process.release();
}

// where the juice is
void run_cheat() {
	using namespace sdk;

	while (!GetAsyncKeyState(VK_INSERT)) {
		if (!interfaces::engine_client.is_in_game()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		mango::logger.info(enc_str("Entering main loop..."));

		// modulate on game load
		features::nightmode::modulate(config::misc::nightmode_color);

		// custom models
		//features::modelchanger::update_player(config::models::player);
		//features::modelchanger::update_knife(config::models::knife);
		//globals::client_state.force_full_update();
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));

		{
			// get localplayer
			const auto local_player = interfaces::client_entity_list.get_local_player();
			const auto weapon = interfaces::client_entity_list.get_client_entity<C_WeaponCSBase>(local_player.m_hActiveWeapon() & 0xFFF);
			const auto sequence_num = globals::process.read<int>(weapon.get_weaponcs_base_addr() + 0x28BC);
			// m_nSequence 0x28BC

			// print every activity for localplayer
			if (const auto model = weapon.get_model()) {
				if (const auto studiohdr = interfaces::model_info.get_studio_hdr(model)) {
					if (const auto seqdesc = studiohdr_t::get_sequence_desc(studiohdr, sequence_num)) {
						const auto sequence = seqdesc();

						char actname[128];
						globals::process.read(uint32_t(&seqdesc) + sequence.szactivitynameindex, actname, 128);
						actname[127] = '\0';

						mango::logger.info(actname, ": ", sequence.activity, ", ", sequence.actweight);
					}
				}
			}
		}

		// we're in game, lets do some shib
		while (interfaces::engine_client.is_in_game()) {
			if (GetAsyncKeyState(VK_INSERT))
				return;

			// get localplayer
			const auto local_player = interfaces::client_entity_list.get_local_player();
			const auto weapon = interfaces::client_entity_list.get_client_entity<C_WeaponCSBase>(local_player.m_hActiveWeapon() & 0xFFF);

			// print every activity for localplayer
			if (const auto model = local_player.get_model()) {
				if (const auto studiohdr = interfaces::model_info.get_studio_hdr(model)) {
					if (const auto seqdesc = studiohdr_t::get_sequence_desc(studiohdr, local_player.m_nSequence())) {
						const auto sequence = seqdesc();

						// label
						char labelname[128];
						globals::process.read(uint32_t(&seqdesc) + sequence.szlabelindex, labelname, 128);
						labelname[127] = '\0';

						// activity name
						char activityname[128];
						globals::process.read(uint32_t(&seqdesc) + sequence.szactivitynameindex, activityname, 128);
						activityname[127] = '\0';

						mango::logger.info("Sequence: ", weapon.m_nSequence(), ", Activity: ", activityname, ", Label: ", labelname, ", Weight: ", sequence.actweight);
					}
				}
			}

			// virtualmodel_t::CUtlVector::m_iSize
			//const auto c = offsetof(virtualmodel_t, m_seq) + offsetof(CUtlVector, m_iSize);

			//mango::logger.info(globals::process.read<int>(weapon.get_weaponcs_base_addr() + 0x28BC));
			//globals::process.write<int>(weapon.get_weaponcs_base_addr() + 0x28BC, 2);

			// noflash
			if (config::misc::noflash_enabled)
				local_player.m_flFlashDuration = 0.f;

			// iterate over every player
			for (int i = 1; i < 64; ++i) {
				const auto player = interfaces::client_entity_list.get_client_entity<C_CSPlayer>(i);
				if (!player.get_client_entity_addr())
					continue;

				// ignore self, dormant players, and dead players
				if (player.get_client_entity_addr() == local_player.get_client_entity_addr() || player.is_dormant() || player.m_iHealth() <= 0)
					continue;

				// enemies
				if (player.m_iTeamNum() != local_player.m_iTeamNum()) {
					// glow
					if (config::glow::enemy_enabled)
						features::glow::draw_player(player, config::glow::enemy_color);

					// radar
					if (config::misc::radar_enabled)
						player.m_bSpotted = true;
				} else /* teammates */ {
					// glow
					if (config::glow::teammate_enabled)
						features::glow::draw_player(player, config::glow::teammate_color);
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}

		mango::logger.info(enc_str("Exiting main loop..."));
	}
}

// entrypoint
int main() {
	setup_logger();

	try {
		setup_cheat();
		mango::ScopeGuard _guard(&release_cheat);
		run_cheat();
	} catch (const std::exception& e) {
		mango::logger.error(e.what());
		std::system(enc_str("pause").c_str());
	}
	
	return 0;
}