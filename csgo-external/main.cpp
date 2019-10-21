#include <epic/process.h>

#include <misc/logger.h>
#include <misc/error_codes.h>


// setup logger channels
void setup_logger() {
	static const auto set_attribute = [](const WORD attribute) {
		static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, attribute);
	};

	// info channel (logger.info(...))
	mango::logger.set_info_channel([](std::stringstream&& ss) {
		std::cout << "[";
		set_attribute(FOREGROUND_BLUE | FOREGROUND_GREEN);
		std::cout << "info";
		set_attribute(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		std::cout << "] " << ss.str() << std::endl;
	});

	// error channel (logger.error(...))
	mango::logger.set_error_channel([](std::stringstream&& ss) {
		std::cout << "[";
		set_attribute(FOREGROUND_RED);
		std::cout << "error";
		set_attribute(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		std::cout << "] " << ss.str() << std::endl;
	});
}

int main() {
	setup_logger();

	try {
		// options for setting up the process
		mango::Process::SetupOptions options;
		options.m_defer_module_loading = true;

		// setup process
		mango::Process process(0x69, options);
	} catch (mango::MangoError& e) {
		mango::logger.error(e.what());
	}

	mango::logger.info("Program ended.");
	getchar();
	return 0;
}