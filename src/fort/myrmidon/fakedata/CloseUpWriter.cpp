#include "CloseUpWriter.hpp"

namespace fort {
namespace myrmidon {


CloseUpWriter::CloseUpWriter(const FrameDrawer::Ptr & drawer)
	: d_drawer(drawer) {
}

CloseUpWriter::~CloseUpWriter() = default;

} // namespace myrmidon
} // namespace fort
