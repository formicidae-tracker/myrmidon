#include "FrameDrawer.hpp"

#include "Config.hpp"

#include <fort/myrmidon/priv/Isometry2D.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace fort {
namespace myrmidon {

cv::Mat TransformFromCenter(double x,double y,double angle, double size) {

	cv::Mat_<double> rot(2,3);

	auto trans = priv::Isometry2Dd(0.0,Eigen::Vector2d(size/2,size/2)) *
		priv::Isometry2Dd(angle,
		                  Eigen::Vector2d(x,y)) *
		priv::Isometry2Dd(0.0,Eigen::Vector2d(-size/2,-size/2));

	auto m = trans.rotation().matrix();
	rot << m(0,0),m(0,1),trans.translation().x(),
		m(1,0),m(1,1),trans.translation().y();

	return rot;
}


FrameDrawer::FrameDrawer(const Config & config) {
	d_AA = 2.0;
	auto [create,destroy]  = fort::tags::GetFamily(fort::tags::Family::Tag36h11);
	d_family = std::shared_ptr<apriltag_family_t>(create(),destroy);

	for ( const auto & [antID,ant] : config.Ants ) {
		d_ants.insert({antID,BuildIndexImage(antID,ant)});
	}
}

void FrameDrawer::Draw(cv::Mat & dest,
                       const IdentifiedFrame & frame) const {
	dest = cv::Mat(frame.Height,frame.Width,CV_8UC1);
	dest.setTo(127);
	cv::Mat tempShape,tempMask;
	for ( size_t i = 0;
	      i < frame.Positions.rows();
	      ++i ) {
		AntID antID = frame.Positions(i,0);
		if ( d_ants.count(antID) == 0 ) {
			continue;
		}
		const AntIndexImage & images = d_ants.at(antID);
		cv::Size tempSize(images.Shape.size().width/d_AA,
		                  images.Shape.size().height/d_AA);

		//first we rotate the shape
		auto rot = TransformFromCenter(0,0,frame.Positions(i,3),images.Shape.rows);
		rot /= d_AA;

		cv::warpAffine(images.Shape,tempShape,rot,tempSize,cv::INTER_LINEAR,cv::BORDER_CONSTANT,127);
		cv::warpAffine(images.Mask,tempMask,rot,tempSize,cv::INTER_LINEAR,cv::BORDER_CONSTANT,0);



	}
}


void FrameDrawer::WriteAnt(cv::Mat & shape,
                           cv::Mat & mask) const {
	std::vector<std::vector<cv::Point2f>> polys =
		{
		 {
		  {0.6,0.5},
		  {0.5,0.4},
		  {0.4,0.5},
		  {0.5,0.6},
		 },
		 {
		  {0.45,0.5},
		  {0.3,0.35},
		  {0.05,0.5},
		  {0.3,0.65},
		 },
		 {
		  {0.55,0.5},
		  {0.65,0.375},
		  {0.8,0.5},
		  {0.65,0.625},
		 },
		};

	for (  auto & points : polys ) {
		std::vector<cv::Point> pp;
		for ( auto & p : points ) {
			pp.push_back(shape.rows*p);
		}
		cv::fillPoly(shape,pp,0);
		cv::fillPoly(mask,pp,255);
	}

}

void FrameDrawer::WriteTag(cv::Mat & image,
                           uint32_t tagID,
                           size_t pixelSize) const {
	uint8_t border(255),inside(0);
	if ( d_family->reversed_border == true ) {
		border = 0;
		inside = 255;
	}
	auto setPixel = [&](size_t x,
	                    size_t y,
	                    uint8_t value) {
		                image(cv::Rect(x*pixelSize,y*pixelSize,pixelSize,pixelSize)).setTo(value);
	                };
	size_t insideStart = (d_family->total_width - d_family->width_at_border)/2;
	image.setTo(border);
	image(cv::Rect(insideStart * pixelSize,
	               insideStart * pixelSize,
	               d_family->width_at_border*pixelSize,
	               d_family->width_at_border*pixelSize)).setTo(inside);

	uint64_t code = d_family->codes[tagID % d_family->ncodes];
	for ( size_t i = 0; i < d_family->nbits; ++i) {
		uint8_t color = (code & 1) ?  255 : 0 ;
		code = code >> 1;
		size_t ii = d_family->nbits - i - 1;
		setPixel(d_family->bit_x[ii]+insideStart,d_family->bit_y[ii]+insideStart,color);
	}


}


FrameDrawer::AntIndexImage FrameDrawer::BuildIndexImage(AntID antID,
                                                        const AntData & ant) const {
	AntIndexImage res = {
	                     .Shape = cv::Mat(d_AA*ant.AntSize,d_AA*ant.AntSize,CV_8UC1),
	                     .Mask = cv::Mat(d_AA*ant.AntSize,d_AA*ant.AntSize,CV_8UC1),
	};
	res.Shape.setTo(127);
	res.Mask.setTo(0);

	WriteAnt(res.Shape,res.Mask);

	cv::Mat tempShape,tempMask;
	cv::Size tempSize(d_AA*ant.AntSize,d_AA*ant.AntSize);

	auto rot = TransformFromCenter(d_AA*ant.AntPose.x(),
	                               d_AA*ant.AntPose.y(),
	                               ant.AntPose.z(),
	                               d_AA*ant.AntSize);

	cv::warpAffine(res.Shape,tempShape,rot,tempSize,cv::INTER_LINEAR,cv::BORDER_CONSTANT,127);
	cv::warpAffine(res.Mask,tempMask,rot,tempSize,cv::INTER_LINEAR,cv::BORDER_CONSTANT,0);


	cv::Rect tagRoi(d_AA*(ant.AntSize-ant.TagSize)/2,
	                d_AA*(ant.AntSize-ant.TagSize)/2,
	                d_AA*ant.TagSize,
	                d_AA*ant.TagSize);
	auto tagShape = tempShape(tagRoi);
	WriteTag(tagShape,antID-1,d_AA*ant.TagSize/d_family->total_width);
	tempMask(tagRoi).setTo(255);

	cv::warpAffine(tempShape,res.Shape,rot,res.Shape.size(),cv::INTER_LINEAR|cv::WARP_INVERSE_MAP,cv::BORDER_CONSTANT,127);
	cv::warpAffine(tempMask,res.Mask,rot,res.Shape.size(),cv::INTER_LINEAR|cv::WARP_INVERSE_MAP,cv::BORDER_CONSTANT,0);

	return res;
}


} // namespace myrmidon
} // namespace fort
