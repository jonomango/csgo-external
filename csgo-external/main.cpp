#include <epic/process.h>

#include <misc/logger.h>
#include <misc/error_codes.h>


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

	mango::logger.success("Setup logger channels.");
}

// just a big try-catch block :P
template <typename Callable>
void surround_try(Callable&& callable) {
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

	surround_try([]() {
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
		options.m_defer_module_loading = true;

		// setup process
		mango::Process process(process_id, options);

		mango::logger.success("Setup process: ", process.get_name());

		// get interfaces
		const auto dump_interfaces = [&](const std::string& module_name) {
			const auto create_interface = uint32_t(process.get_proc_addr(module_name, "CreateInterface"));
			if (!create_interface)
				throw std::runtime_error("Failed to get CreateInterface for " + module_name);

			// follow the jmp in CreateInterface to get this (or just CreateInterface - 0x70)
			const auto create_interface_internal = create_interface + process.read<uint32_t>(create_interface + 0x5) + 0x9;

			// linked list
			struct InterfaceReg {
				uint32_t m_create_fn;
				uint32_t m_name;
				uint32_t m_next;
			};

			// mov esi, interface_list
			auto curr_interface_ptr = process.read<uint32_t>(process.read<uint32_t>(create_interface_internal + 0x6));

			// traverse the linked list of interfaces
			while (curr_interface_ptr) {
				const auto iface = process.read<InterfaceReg>(curr_interface_ptr);

				// read the name
				char name[128];
				process.read(iface.m_name, name, 128);
				mango::logger.info(name);

				curr_interface_ptr = iface.m_next;
			}
		};

		//dump_interfaces("engine.dll");

		dump_interfaces("client_panorama.dll");
		// get netvars
	});

	mango::logger.info("Program ended.");
	getchar();
	return 0;
}