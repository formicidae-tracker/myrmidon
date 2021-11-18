#pragma once

#include <memory>
#include <utility>
#include <cstdint>

#include <Eigen/Core>

#include <fort/tags/fort-tags.hpp>

#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>

#include "Config.hpp"

namespace cv {
class Mat;
}

namespace fort {
namespace myrmidon {



namespace priv {
template<typename T> class Isometry2D;
}

class AntData;

class FrameDrawer {
public :
	typedef std::shared_ptr<FrameDrawer> Ptr;

	FrameDrawer(fort::tags::Family family,
	            const Config & config);

	void Draw(cv::Mat & dest,
	          const IdentifiedFrame & frame) const;

	void ComputeTagPosition(Eigen::Vector2d & position,
	                        double & angle,
	                        AntID antID,
	                        const Eigen::Vector3d & antPosition);

	void ComputeCorners(Vector2dList & results,
	                    AntID antID,
	                    const Eigen::Vector3d & antPosition);


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
	              uint8_t gray,
	              size_t antSize) const;


	Config                       d_config;
	std::map<AntID,ColoredShape> d_ants;

	std::shared_ptr<apriltag_family_t> d_family;

};

} // namespace myrmidon
} // namespace fort
