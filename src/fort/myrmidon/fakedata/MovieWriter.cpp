#include "MovieWriter.hpp"

namespace fort {
namespace myrmidon {

MovieWriter::MovieWriter(const FrameDrawer::Ptr & drawer)
	: d_drawer(drawer) {
}

MovieWriter::~MovieWriter() {
}




} // namespace myrmidon
} // namespace fort
