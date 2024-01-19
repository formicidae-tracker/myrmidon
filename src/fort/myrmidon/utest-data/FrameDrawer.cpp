#include "FrameDrawer.hpp"

#include "Config.hpp"

#include <fort/myrmidon/priv/Isometry2D.hpp>

#include <memory>
#include <stdexcept>

#include <fort/video/Frame.hpp>
#include <fort/video/TypesIO.hpp>
#include <string>

namespace fort {
namespace myrmidon {

FrameDrawer::FrameDrawer(fort::tags::Family family, const Config &config)
    : d_config(config) {
	auto [create, destroy] = fort::tags::GetFamily(family);
	d_family = std::shared_ptr<apriltag_family_t>(create(), destroy);

	for (const auto &[antID, ant] : config.Ants) {
		d_ants.insert({antID, BuildAntShape(antID, ant)});
	}
}

void FrameDrawer::Draw(video::Frame &buffer, const IdentifiedFrame &frame)
    const {
	video::Resolution wantedResolution{
	    static_cast<int>(frame.Width),
	    static_cast<int>(frame.Height),
	};

	if (buffer.Size != wantedResolution || buffer.Format != AV_PIX_FMT_GRAY8) {
		throw std::invalid_argument{
		    "Output buffer must be a " + std::to_string(wantedResolution) +
		    " buffer GRAY8 image, got a " + std::to_string(buffer.Size) + " " +
		    std::to_string(buffer.Format) + " buffer"};
	}

	// fills background
	memset(buffer.Planes[0], 127, buffer.Linesize[0] * frame.Height);

	// draw shapes at the right position
	for (size_t i = 0; i < frame.Positions.rows(); ++i) {
		AntID antID = frame.Positions(i, 0);
		if (d_ants.count(antID) == 0) {
			continue;
		}
		auto transform = priv::Isometry2Dd(
		    frame.Positions(i, 3),
		    frame.Positions.block<1, 2>(i, 1).transpose()
		);

		DrawShapeOnImage(buffer, d_ants.at(antID), transform);
	}
}

void FrameDrawer::WriteAnt(ColoredShape &shape, uint8_t gray, size_t antSize)
    const {
	std::vector<Vector2dList> polys = {
	    {
	        {0.1, 0.0},
	        {0.0, -0.1},
	        {-0.1, 0.0},
	        {0.0, 0.1},
	    },
	    {
	        {-0.05, 0.0},
	        {-0.2, -0.15},
	        {-0.45, 0.0},
	        {-0.2, 0.15},
	    },
	    {
	        {0.05, 0.0},
	        {0.15, -0.125},
	        {0.3, 0.0},
	        {0.15, 0.125},
	    },
	};

	for (auto &poly : polys) {
		shape.push_back({gray, Vector2dList()});
		shape.back().second.reserve(poly.size());
		for (const auto &p : poly) {
			shape.back().second.push_back(p * antSize);
		}
	}
}

void FrameDrawer::WriteTag(
    ColoredShape           &shape,
    uint32_t                tagID,
    const priv::Isometry2Dd tagToAnt,
    size_t                  pixelSize
) const {
	uint8_t border(255), inside(0);
	if (d_family->reversed_border == true) {
		border = 0;
		inside = 255;
	}

	auto setQuad = [&](double x, double y, double w, double h, uint8_t value) {
		shape.push_back(
		    {value, {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}}}
		);
		for (auto &p : shape.back().second) {
			p *= pixelSize;
			p = tagToAnt * p;
		}
	};

	int offset = d_family->total_width / 2;
	offset *= -1;
	setQuad(
	    offset,
	    offset,
	    d_family->total_width,
	    d_family->total_width,
	    border
	);
	offset += (d_family->total_width - d_family->width_at_border) / 2;
	setQuad(
	    offset,
	    offset,
	    d_family->width_at_border,
	    d_family->width_at_border,
	    inside
	);

	uint64_t code = d_family->codes[tagID % d_family->ncodes];
	for (size_t i = 0; i < d_family->nbits; ++i) {
		uint8_t color = (code & 1) ? 255 : 0;
		code          = code >> 1;
		size_t ii     = d_family->nbits - i - 1;
		setQuad(
		    int(d_family->bit_x[ii]) + offset,
		    int(d_family->bit_y[ii]) + offset,
		    1,
		    1,
		    color
		);
	}
}

FrameDrawer::ColoredShape
FrameDrawer::BuildAntShape(AntID antID, const AntData &ant) const {
	ColoredShape res;
	WriteAnt(res, ant.Color, ant.AntSize);

	auto tagToAnt =
	    priv::Isometry2Dd(ant.AntPose.z(), ant.AntPose.block<2, 1>(0, 0))
	        .inverse();
	WriteTag(res, antID - 1, tagToAnt, ant.TagSize / d_family->total_width);

	return res;
}

std::tuple<Eigen::Vector2i, Eigen::Vector2i>
computeAABB(const std::vector<Eigen::Vector2d> &vertices) {
	Eigen::Vector2i pMin{
	    std::numeric_limits<int>::max(),
	    std::numeric_limits<int>::max(),
	},
	    pMax{
	        std::numeric_limits<int>::min(),
	        std::numeric_limits<int>::min(),
	    };
	for (const auto v : vertices) {
		pMin.x() = std::min(pMin.x(), int(std::floor(v.x())));
		pMin.y() = std::min(pMin.y(), int(std::ceil(v.y())));
		pMax.x() = std::max(pMax.x(), int(std::floor(v.x())));
		pMax.y() = std::max(pMax.y(), int(std::ceil(v.y())));
	}
	return {pMin, pMax};
}

int sideOfEdge(
    const Eigen::Vector2d &a, const Eigen::Vector2d &b, const Eigen::Vector2d &p
) {
	return (b.x() - a.x()) * (p.y() - a.y()) -
	       (p.x() - a.x()) * (b.y() - a.y());
}

bool isInside(
    const std::vector<Eigen::Vector2d> &vertices, const Eigen::Vector2d &p
) {
	int windingNumber{0};

	for (size_t i = 0; i < vertices.size(); ++i) {
		size_t j = (i + 1) % vertices.size();

		if (vertices[i].y() <= p.y()) {
			if ((vertices[j].y() > p.y()) &&
			    sideOfEdge(vertices[i], vertices[j], p) > 0) {
				++windingNumber;
			}
		} else if (vertices[j].y() <= p.y() && sideOfEdge(vertices[i], vertices[j], p) < 0) {
			--windingNumber;
		}
	}
	return windingNumber % 2 != 0;
}

void fillConvexPoly(
    video::Frame                       &img,
    const std::vector<Eigen::Vector2d> &vertices,
    uint8_t                             color
) {
	auto [min, max] = computeAABB(vertices);
	for (int y = min.y(); y < max.y(); y++) {
		for (int x = min.x(); x < max.x(); x++) {
			Eigen::Vector2d p{x, y};
			if (isInside(vertices, p) == false) {
				continue;
			}
			img.Planes[0][y * img.Linesize[0] + x] = color;
		}
	}
}

void FrameDrawer::DrawShapeOnImage(
    video::Frame                   &dest,
    const ColoredShape             &shape,
    const priv::Isometry2D<double> &transformation
) {
	std::vector<Eigen::Vector2d> vertices;
	for (const auto &[color, poly] : shape) {
		vertices.clear();
		vertices.reserve(poly.size());
		for (const auto &p : poly) {
			vertices.push_back(transformation * p);
		}
		fillConvexPoly(dest, vertices, color);
	}
}

void FrameDrawer::ComputeTagPosition(
    Eigen::Vector2d       &position,
    double                &angle,
    AntID                  antID,
    const Eigen::Vector3d &antPosition
) {
	auto &ant = d_config.Ants.at(antID);
	auto  transform =
	    priv::Isometry2Dd(antPosition.z(), antPosition.block<2, 1>(0, 0));

	auto tagToOrig =
	    transform *
	    priv::Isometry2Dd(ant.AntPose.z(), ant.AntPose.block<2, 1>(0, 0))
	        .inverse();
	position = tagToOrig.translation();
	angle    = tagToOrig.angle();
}

void FrameDrawer::ComputeCorners(
    Vector2dList &results, AntID antID, const Eigen::Vector3d &antPosition
) {

	auto transform =
	    priv::Isometry2Dd(antPosition.z(), antPosition.block<2, 1>(0, 0));
	auto &shapes = d_ants.at(antID)[4].second;

	results.resize(4);
	results[0] = transform * shapes[3];
	results[1] = transform * shapes[2];
	results[2] = transform * shapes[1];
	results[3] = transform * shapes[0];
}

} // namespace myrmidon
} // namespace fort
