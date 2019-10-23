#include <epic/process.h>
#include <epic/shellcode.h>

#include <misc/logger.h>
#include <misc/vector.h>
#include <misc/error_codes.h>

#include "sdk/misc/constants.h"


// setup logger channels
void setup_logger() {
	static const auto display_info = [](const uint16_t attribute, const std::string_view prefix, std::stringstream&& ss) {
		static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

		std::cout << "[";
		SetConsoleTextAttribute(handle, attribute);
		std::cout << prefix;
		SetConsoleTextAttribute(handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		std::cout << "] " << ss.str() << std::endl;
	};

	// info channel
	mango::logger.set_info_channel([](std::stringstream&& ss) {
		display_info(FOREGROUND_BLUE | FOREGROUND_GREEN, "info", std::move(ss));
	});

	// success channel
	mango::logger.set_success_channel([](std::stringstream&& ss) {
		display_info(FOREGROUND_GREEN, "success", std::move(ss));
	});

	// error channel
	mango::logger.set_error_channel([](std::stringstream&& ss) {
		display_info(FOREGROUND_RED, "error", std::move(ss));
	});

	mango::logger.success("Logging channels initialized.");
}

// just a big try-catch block :P
template <typename Callable>
void surround_try_block(Callable&& callable) {
	try {
		std::invoke(callable);
	} catch (mango::MangoError & e) {
		mango::logger.error("Unhandled MangoError: ", e.what());
	} catch (std::exception & e) {
		mango::logger.error("Unhandled exception: ", e.what());
	} catch (...) {
		mango::logger.error("Unhandled exception");
	}
}

int main() {
	setup_logger();

	surround_try_block([]() {
		// get window
		const auto csgo_hwnd = FindWindow(nullptr, "Counter-Strike: Global Offensive");
		if (!csgo_hwnd)
			throw std::runtime_error("Failed to find csgo's window");

		// get process id from window handle
		DWORD process_id;
		if (!GetWindowThreadProcessId(csgo_hwnd, &process_id))
			throw std::runtime_error("Failed to get process id");

		mango::logger.success("Found Process ID: 0x", std::hex, std::uppercase, process_id);

		// options for setting up the process
		mango::Process::SetupOptions options;
		options.m_defer_module_loading = false;

		// setup process
		mango::logger.info("Setting up process...");
		sdk::globals::process.setup(process_id, options);
		mango::logger.success("Process initialized: ", sdk::globals::process.get_name());

		// gets interfaces and stuffs and shibs
		sdk::setup_constants();

		// cool stuffs
		while (sdk::globals::engine_client.is_in_game()) {
			const auto local_player = sdk::globals::client_entity_list.get_local_player();

			sdk::globals::engine_client.set_view_angles({ 0.f, 0.f, 0.f });
		}
	});

	mango::logger.info("Program ended.");
	getchar();
	return 0;
}