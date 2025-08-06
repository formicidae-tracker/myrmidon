#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include <cpptrace/cpptrace.hpp>

int main(int argc, char **argv) {
	if (argc < 2) {
		return 0;
	}
	auto output = std::fopen(argv[1], "r");
	if (output == nullptr) {
		std::cerr << "Could not open '" << argv[1] << "': " << errno << " "
		          << strerror(errno) << std::endl;
		return 1;
	}

	cpptrace::object_trace trace;
	while (true) {
		cpptrace::safe_object_frame frame;
		// fread used over read because a read() from a pipe might not read the
		// full frame
		std::size_t res = fread(&frame, sizeof(frame), 1, output);
		if (std::feof(output)) {
			break;
		}

		if (res == 0) {
			break;
		} else if (res != 1) {
			std::cerr << "Something went wrong while reading from the pipe"
			          << res << std::endl;
			break;
		} else {
			std::cerr << "address:" << std::hex << frame.raw_address
			          << " offset:" << std::hex
			          << frame.address_relative_to_object_start << " path:'"
			          << std::string(frame.object_path) << "'" << std::endl;
			try {
				trace.frames.push_back(frame.resolve());
			} catch (const std::exception &e) {
				std::cerr << "unable to resolve frame: " << e.what()
				          << std::endl;
			}
		}
	}
	trace.resolve().print();
	return 0;
}
