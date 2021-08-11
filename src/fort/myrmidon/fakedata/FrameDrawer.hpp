#pragma once

#include <fort/tags/fort-tags.hpp>

#include <fort/myrmidon/Types.hpp>

#include <opencv2/core.hpp>



namespace fort {
namespace myrmidon {

class Config;
class AntData;

class FrameDrawer {
public :

	FrameDrawer(const Config & config);

	void Draw(cv::Mat & dest,
	          const IdentifiedFrame & frame) const;

private :
	struct AntIndexImage {
		cv::Mat Shape,Mask;
	};

	AntIndexImage BuildIndexImage(AntID antID,
	                              const AntData & ant) const;

	void WriteTag(cv::Mat & image,
	              uint32_t tagID,
	              size_t pixelSize) const;

	void WriteAnt(cv::Mat & shape,
	              cv::Mat & mask) const;


	std::map<AntID,AntIndexImage> d_ants;

	std::shared_ptr<apriltag_family_t> d_family;
	double d_AA;
};

} // namespace myrmidon
} // namespace fort
