#include "CloseUpWriter.hpp"
#include "fort/myrmidon/priv/PNGUtils.hpp"

#include <fort/myrmidon/priv/TagCloseUp.hpp>

#include <fort/video/Frame.hpp>
#include <libavutil/pixfmt.h>
#include <stdexcept>

namespace fort {
namespace myrmidon {

CloseUpWriter::CloseUpWriter(
    UTestData::TDDInfo &tddInfo, const FrameDrawer::Ptr &drawer
)
    : d_tddInfo(tddInfo)
    , d_drawer(drawer) {
	fs::create_directories(d_tddInfo.AbsoluteFilePath / "ants");
}

CloseUpWriter::~CloseUpWriter() = default;

void CloseUpWriter::Prepare(size_t index) {
	d_fullFrameNeeded = d_tddInfo.HasFullFrame;
	d_seen.clear();
}

void CloseUpWriter::WriteFrom(const IdentifiedFrame &data, uint64_t frameID) {
	std::map<AntID, Eigen::Vector2f> neededCloseUp;

	for (size_t i = 0; i < data.Positions.rows(); ++i) {
		AntID antID = data.Positions(i, 0);
		if (d_seen.count(antID) == 0) {
			Eigen::Vector2d position;
			double          angle;
			d_drawer->ComputeTagPosition(
			    position,
			    angle,
			    antID,
			    data.Positions.block<1, 3>(i, 1).transpose()
			);

			neededCloseUp[antID] = Eigen::Vector2f(position.x(), position.y());
		}
	}

	if (d_fullFrameNeeded == false && neededCloseUp.empty()) {
		return;
	}

	video::Frame frameBuffer{
	    int(data.Width),
	    int(data.Height),
	    AV_PIX_FMT_GRAY8};

	d_drawer->Draw(frameBuffer, data);

	if (d_fullFrameNeeded == true) {
		d_fullFrameNeeded = false;
		SaveFullFrame(frameBuffer, frameID);
		SaveExpectedFullFrame(data, frameID);
	}

	for (const auto &[antID, position] : neededCloseUp) {
		d_seen.insert(antID);
		SaveCloseUp(frameBuffer, frameID, antID, position);
		SaveExpectedCloseUpFrame(data, frameID, antID);
	}
}

void CloseUpWriter::Finalize(size_t index, bool last) {
	std::sort(
	    d_tddInfo.TagCloseUps.begin(),
	    d_tddInfo.TagCloseUps.end(),
	    [](const priv::TagCloseUp::ConstPtr &a,
	       const priv::TagCloseUp::ConstPtr &b) {
		    if (a->AbsoluteFilePath() == b->AbsoluteFilePath()) {
			    return a->TagValue() < b->TagValue();
		    }
		    return a->AbsoluteFilePath() < b->AbsoluteFilePath();
	    }
	);
}

std::string CloseUpWriter::FullFramePath(uint64_t frameID) const {
	return d_tddInfo.AbsoluteFilePath / "ants" /
	       ("frame_" + std::to_string(frameID) + ".png");
}

std::string CloseUpWriter::CloseUpPath(uint64_t frameID, AntID antID) const {
	return d_tddInfo.AbsoluteFilePath / "ants" /
	       ("ant_" + std::to_string(antID - 1) + "_frame_" +
	        std::to_string(frameID) + ".png");
}

image_u8_t AsImageU8(const video::Frame &frame) {
	if (frame.Format != AV_PIX_FMT_GRAY8) {
		throw std::invalid_argument{"Invalid video::Frame format"};
	}
	return image_u8_t{
	    .width  = std::get<0>(frame.Size),
	    .height = std::get<1>(frame.Size),
	    .stride = frame.Linesize[0],
	    .buf    = frame.Planes[0],
	};
}

void CloseUpWriter::SaveFullFrame(const video::Frame &frame, uint64_t frameID) {
	priv::WritePNG(FullFramePath(frameID), AsImageU8(frame));
}

struct ROI {
	int X, Y, W, H;
	ROI(const Eigen::Vector2i &position, const Eigen::Vector2i &size)
	    : X{position.x()}
	    , Y{position.y()}
	    , W{size.x()}
	    , H{size.y()} {};
};

image_u8_t GetROI(image_u8_t image, const ROI &roi) {
	return image_u8_t{
	    .width  = roi.W,
	    .height = roi.H,
	    .stride = image.stride - roi.X,
	    .buf    = image.buf + roi.Y * image.stride + roi.X,
	};
}

void CloseUpWriter::SaveCloseUp(
    const video::Frame    &frame,
    uint64_t               frameID,
    AntID                  antID,
    const Eigen::Vector2f &position
) {

	auto roi = ROI{
	    (position - Eigen::Vector2f(150, 150)).cast<int>(),
	    Eigen::Vector2i(300, 300),
	};
	roi.X = std::clamp(roi.X, 0, std::get<0>(frame.Size) - 300);
	roi.Y = std::clamp(roi.Y, 0, std::get<1>(frame.Size) - 300);

	priv::WritePNG(CloseUpPath(frameID, antID), GetROI(AsImageU8(frame), roi));
}

void CloseUpWriter::SaveExpectedFullFrame(
    const IdentifiedFrame &data, uint64_t frameID
) {
	auto path = FullFramePath(frameID);
	d_tddInfo.TagCloseUpFiles.insert({frameID, {path, nullptr}});
	for (size_t i = 0; i < data.Positions.rows(); ++i) {
		SaveExpectedCloseUp(path, data, frameID, data.Positions(i, 0), true);
	}
}

void CloseUpWriter::SaveExpectedCloseUpFrame(
    const IdentifiedFrame &data, uint64_t frameID, AntID antID
) {
	auto path = CloseUpPath(frameID, antID);
	d_tddInfo.TagCloseUpFiles.insert(
	    {frameID, {path, std::make_shared<AntID>(antID - 1)}}
	);
	SaveExpectedCloseUp(path, data, frameID, antID, false);
}

void CloseUpWriter::SaveExpectedCloseUp(
    const fs::path        &path,
    const IdentifiedFrame &data,
    uint64_t               frameID,
    AntID                  antID,
    bool                   fullFrame
) {
	int index = -1;
	for (size_t i = 0; i < data.Positions.rows(); ++i) {
		if (data.Positions(i, 0) == antID) {
			index = i;
			break;
		}
	}

	if (index < 0) {
		throw std::runtime_error("could not find ant " + std::to_string(antID));
	}

	Eigen::Vector2d position;
	double          angle;
	Vector2dList    corners;

	auto antPosition = data.Positions.block<1, 3>(index, 1).transpose();
	d_drawer->ComputeCorners(corners, antID, antPosition);
	d_drawer->ComputeTagPosition(position, angle, antID, antPosition);

	if (fullFrame == false) {
		Eigen::Vector2d offset;
		offset.x() = std::clamp(150.0 - position.x(), 300.0 - data.Width, 0.0);
		offset.y() = std::clamp(150.0 - position.y(), 300.0 - data.Height, 0.0);
		position += offset;
		for (auto &p : corners) {
			p += offset;
		}
	}

	priv::FrameReference ref(
	    d_tddInfo.AbsoluteFilePath.filename(),
	    frameID,
	    data.FrameTime
	);
	auto tcu = std::make_shared<priv::TagCloseUp>(
	    path,
	    ref,
	    antID - 1,
	    position,
	    angle,
	    corners
	);
	d_tddInfo.TagCloseUps.push_back(tcu);
}

} // namespace myrmidon
} // namespace fort
