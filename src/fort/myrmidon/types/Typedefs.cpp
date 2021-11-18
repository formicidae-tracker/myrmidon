#include "Typedefs.hpp"

#include <iomanip>

namespace fort {
namespace myrmidon {

std::string FormatTagID(TagID tagID) {
	std::ostringstream oss;
	oss << "0x" << std::hex << std::setfill('0') << std::setw(3) << tagID;
	return oss.str();
}

std::string FormatAntID(fort::myrmidon::AntID ID) {
	std::ostringstream os;
	os << std::setw(3) << std::setfill('0') << ID;
	return os.str();
}

} // namespace myrmidon
} // namespace fort
