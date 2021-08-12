#pragma once

#include <fort/tags/fort-tags.hpp>

#include <fort/myrmidon/Types.hpp>

#include <opencv2/core.hpp>



namespace fort {
namespace myrmidon {

namespace priv {
template<typename T> class Isometry2D;
}

class Config;
class AntData;

class FrameDrawer {
public :

	FrameDrawer(const Config & config);

	void Draw(cv::Mat & dest,
	          const IdentifiedFrame & frame) const;

private :
	typedef std::vector<std::pair<uint8_t,Vector2dList>> ColoredShape;


	static void DrawShapeOnImage(cv::Mat & dest,
	                             const ColoredShape & shape,
	                             const priv::Isometry2D<double> & transformation);

	ColoredShape BuildAntShape(AntID antID,
	                           const AntData & ant) const;

	void WriteTag(ColoredShape & shape,
	              uint32_t tagID,
	              const priv::Isometry2D<double> tagToAnt,
	              size_t pixelSize) const;

	void WriteAnt(ColoredShape & shape,
	              size_t antSize) const;


	std::map<AntID,ColoredShape> d_ants;

	std::shared_ptr<apriltag_family_t> d_family;

};

} // namespace myrmidon
} // namespace fort
