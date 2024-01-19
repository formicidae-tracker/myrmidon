#include "Video.hpp"

#include <fort/video/Reader.hpp>

namespace fort {
namespace myrmidon {

void VideoSequence::ForEach(
    const VideoSegment::List &list,
    std::function<void(const video::Frame &frame, const VideoFrameData &data)>
        operation
) {
	for (const auto &s : list) {

		video::Reader reader{s.AbsoluteFilePath};
		reader.SeekFrame(s.Begin);
		auto iter  = s.Data.cbegin();
		auto frame = reader.CreateFrame(32);
		for (auto moviePos = s.Begin; moviePos < s.End; ++moviePos) {
			if (!reader.Read(*frame)) {
				break;
			}
			while (iter != s.Data.end() && iter->Position < moviePos) {
				++iter;
			}
			if (iter != s.Data.end() && iter->Position == moviePos) {
				operation(*frame, *iter);
			} else {
				operation(
				    *frame,
				    {.Position = moviePos, .Time = Time::SinceEver()}
				);
			}
		}
	}
}

} // namespace myrmidon
} // namespace fort
