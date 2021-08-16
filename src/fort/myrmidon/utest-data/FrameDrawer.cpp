#include "FrameDrawer.hpp"

#include "Config.hpp"

#include <fort/myrmidon/priv/Isometry2D.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace fort {
namespace myrmidon {


FrameDrawer::FrameDrawer(fort::tags::Family family,
                         const Config & config) {
	auto [create,destroy]  = fort::tags::GetFamily(family);
	d_family = std::shared_ptr<apriltag_family_t>(create(),destroy);

	for ( const auto & [antID,ant] : config.Ants ) {
		d_ants.insert({antID,BuildAntShape(antID,ant)});
	}
}

void FrameDrawer::Draw(cv::Mat & dest,
                       const IdentifiedFrame & frame) const {
	// sets the right type
	if ( dest.type() != CV_8UC1 ||
	     dest.size() != cv::Size(frame.Width,frame.Height) ) {
		dest = cv::Mat(frame.Height,frame.Width,CV_8UC1);
	}
	//fills background
	dest.setTo(127);

	//draw shapes at the right position
	for ( size_t i = 0;
	      i < frame.Positions.rows();
	      ++i ) {
		AntID antID = frame.Positions(i,0);
		if ( d_ants.count(antID) == 0 ) {
			continue;
		}
		auto transform = priv::Isometry2Dd(frame.Positions(i,3),
		                                   frame.Positions.block<1,2>(i,1).transpose());

		DrawShapeOnImage(dest,
		                 d_ants.at(antID),
		                 transform);

	}
}


void FrameDrawer::WriteAnt(ColoredShape & shape,
                           size_t antSize) const {
	std::vector<Vector2dList> polys =
		{
		 {
		  {0.1,0.0},
		  {0.0,-0.1},
		  {-0.1,0.0},
		  {0.0,0.1},
		 },
		 {
		  {-0.05,0.0},
		  {-0.2,-0.15},
		  {-0.45,0.0},
		  {-0.2,0.15},
		 },
		 {
		  {0.05,0.0},
		  {0.15,-0.125},
		  {0.3,0.0},
		  {0.15,0.125},
		 },
		};

	for (  auto & poly : polys ) {
		shape.push_back({0,Vector2dList()});
		shape.back().second.reserve(poly.size());
		for ( const auto & p : poly ) {
			shape.back().second.push_back(p*antSize);
		}
	}
}

void FrameDrawer::WriteTag(ColoredShape & shape,
                           uint32_t tagID,
                           const priv::Isometry2Dd tagToAnt,
                           size_t pixelSize) const {
	uint8_t border(255),inside(0);
	if ( d_family->reversed_border == true ) {
		border = 0;
		inside = 255;
	}



	auto setQuad =
		[&](double x,
		    double y,
		    double w,
		    double h,
		    uint8_t value) {
			shape.push_back({value,{{x,y},{x+w,y},{x+w,y+h},{x,y+h}}});
			for ( auto & p : shape.back().second ) {
				p *= pixelSize;
				p = tagToAnt * p ;
			}
		};

	int offset = d_family->total_width / 2;
	offset *= -1;
	setQuad(offset,offset,d_family->total_width,d_family->total_width,border);
	offset += (d_family->total_width - d_family->width_at_border)/2;
	setQuad(offset,offset,d_family->width_at_border,d_family->width_at_border,inside);


	uint64_t code = d_family->codes[tagID % d_family->ncodes];
	for ( size_t i = 0; i < d_family->nbits; ++i) {
		uint8_t color = (code & 1) ?  255 : 0 ;
		code = code >> 1;
		size_t ii = d_family->nbits - i - 1;
		setQuad(int(d_family->bit_x[ii])+offset,
		        int(d_family->bit_y[ii])+offset,
		        1,
		        1,
		        color);
	}

}


FrameDrawer::ColoredShape FrameDrawer::BuildAntShape(AntID antID,
                                                     const AntData & ant) const {
	ColoredShape res;
	WriteAnt(res,ant.AntSize);

	auto tagToAnt = priv::Isometry2Dd(ant.AntPose.z(),ant.AntPose.block<2,1>(0,0)).inverse();
	WriteTag(res,
	         antID-1,
	         tagToAnt,
	         ant.TagSize/d_family->total_width);

	return res;
}

void FrameDrawer::DrawShapeOnImage(cv::Mat & dest,
                                   const ColoredShape & shape,
                                   const priv::Isometry2D<double> & transformation) {
	std::vector<cv::Point> vertices;
	for ( const auto & [color,poly] : shape ) {
		vertices.clear();
		vertices.reserve(poly.size());
		for ( const auto & p : poly ) {
			auto v = transformation * p;
			vertices.push_back(cv::Point(v.x(),v.y()));
		}
		cv::fillConvexPoly(dest,vertices,color);
	}
}



} // namespace myrmidon
} // namespace fort
