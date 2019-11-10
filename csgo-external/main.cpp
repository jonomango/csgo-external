#include <epic/process.h>
#include <epic/shellcode.h>
#include <epic/pattern_scanner.h>
#include <misc/logger.h>
#include <misc/vector.h>
#include <misc/error_codes.h>
#include <crypto/fnv_hash.h>
#include <crypto/string_encryption.h>

#include "sdk/misc/constants.h"
#include "sdk/classes/base_entity.h"

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

// sets up stuff
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
}

// where the juice is
void run_cheat() {
	while (true) {
		if (!sdk::interfaces::engine_client.is_in_game()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		mango::logger.info(enc_str("InGame:true"));

		// update local_player every time we join a game
		const auto local_player = sdk::interfaces::client_entity_list.get_local_player();

		// we're in game, lets do some shib
		while (sdk::interfaces::engine_client.is_in_game()) {
			// iterate over every player
			for (int i = 1; i < 64; ++i) {
				const auto entity = sdk::interfaces::client_entity_list.get_client_entity(i);
				if (!entity || entity.is_dormant() || entity.get_team() == local_player.get_team() || entity.get_health() <= 0)
					continue;

				entity.set_spotted(true);
			}
		}

		mango::logger.info(enc_str("InGame:false"));
	}
}

// entrypoint
int main() {
	setup_logger();
	
	try {
		setup_cheat();
		run_cheat();
	} catch (const mango::MangoError& e) {
		mango::logger.error(e.what());
	} catch (const std::exception& e) {
		mango::logger.error(e.what());
	} catch (...) {
		mango::logger.error(enc_str("Unhandled exception"));
	}
	
	mango::logger.info(enc_str("Program ended."));
	getchar();
	return 0;
}