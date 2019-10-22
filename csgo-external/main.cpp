#include <epic/process.h>
#include <epic/shellcode.h>

#include <misc/logger.h>
#include <misc/error_codes.h>

#include "sdk/interfaces.h"


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

	mango::logger.success("Logger channels initialized.");
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
		mango::Process process(process_id, options);
		mango::logger.success("Process initialized: ", process.get_name());

		// get interfaces
		sdk::InterfaceCache interface_cache(process);
		const auto client_interface = interface_cache.get_interface("client_panorama.dll", "VClient");
		const auto engine_interface = interface_cache.get_interface("engine.dll", "VEngineClient");

		const auto ret_value = process.alloc_virt_mem(4);

		mango::Shellcode(
			"\xB9", engine_interface, // mov ecx, engine_interface
			"\xB8", process.get_vfunc<uint32_t>(engine_interface, 12), // mov eax, vfunc(engine, 12)
			"\xFF\xD0", // call eax
			"\xA3", uint32_t(ret_value), // mov [ret_value], eax
			"\xC3" // ret
		).execute(process);

		mango::logger.info("localplayer index: ", process.read<int>(ret_value));
		process.free_virt_mem(ret_value);
	});

	mango::logger.info("Program ended.");
	getchar();
	return 0;
}