#include <epic/process.h>
#include <epic/shellcode.h>
#include <epic/pattern_scanner.h>
#include <misc/logger.h>
#include <misc/vector.h>
#include <misc/matrix.h>
#include <misc/error_codes.h>
#include <misc/fnv_hash.h>
#include <crypto/string_encryption.h>

#include "sdk/misc/constants.h"
#include "sdk/classes/base_combat_weapon.h"
#include "sdk/classes/studio.h"

#include "hooks/hooks.h"
#include "features/glow.h"
#include "features/nightmode.h"

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
	// get window
	const auto csgo_hwnd = FindWindow(nullptr, enc_str("Counter-Strike: Global Offensive").c_str());
	if (!csgo_hwnd)
		throw std::runtime_error(enc_str("Failed to find csgo's window"));

	// get process id from window handle
	DWORD process_id;
	if (!GetWindowThreadProcessId(csgo_hwnd, &process_id))
		throw std::runtime_error(enc_str("Failed to get process id"));

	mango::logger.success(enc_str("Found Process ID: 0x"), std::hex, std::uppercase, process_id);

	// options for setting up the process
	mango::Process::SetupOptions options;
	options.m_defer_module_loading = false;

	// setup process
	mango::logger.info(enc_str("Setting up process..."));
	sdk::globals::process.setup(process_id, options);
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

	while (!GetAsyncKeyState(VK_INSERT)) try {
		if (!interfaces::engine_client.is_in_game()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		mango::logger.info(enc_str("Entering main loop..."));

		// update local_player every time we join a game
		const auto local_player = interfaces::client_entity_list.get_local_player();

		// modulate on game load
		features::nightmode::modulate(config::misc::nightmode_color);

		// we're in game, lets do some shib
		while (interfaces::engine_client.is_in_game()) { 
			if (GetAsyncKeyState(VK_INSERT))
				return;

			// noflash
			if (config::misc::noflash_enabled)
				local_player.set_flash_duration(0.f);

			// cache the value (barely helps but whatev)
			const auto local_player_team = local_player.get_team();

			// this updates often (probably when objects are added/removed)
			globals::glow_object_manager.update_object_definitions();

			// iterate over every player
			for (int i = 1; i < 64; ++i) {
				const auto entity = interfaces::client_entity_list.get_client_entity(i);
				if (!entity || entity == local_player || entity.is_dormant() || entity.get_health() <= 0)
					continue;

				// enemies
				if (entity.get_team() != local_player_team) {
					// glow
					if (config::glow::enemy_enabled)
						features::glow::draw_entity(entity, config::glow::enemy_color);

					// radar
					if (config::misc::radar_enabled)
						entity.set_spotted(true);
				} else /* teammates */ {
					// glow
					if (config::glow::teammate_enabled)
						features::glow::draw_entity(entity, config::glow::teammate_color);
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}

		mango::logger.info(enc_str("Exiting main loop..."));
	} catch (mango::MangoError& e) {
		mango::logger.error(e.what());
		mango::logger.info(enc_str("Ignoring error..."));
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