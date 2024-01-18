#pragma once

#include "KDTree.hpp"
#include <memory>

#include <fort/video/Frame.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

using Color = std::tuple<uint8_t, uint8_t, uint8_t>;

template <typename T>
void drawLine(
    const video::Frame           &frame,
    const Eigen::Matrix<T, 2, 1> &a,
    const Eigen::Matrix<T, 2, 1> &b,
    const Color                  &color,
    int                           thickness
) {}

class KDTreePrinter {
public:
	static std::unique_ptr<video::Frame>
	Print(const KDTree<int, double, 2>::ConstPtr &tree) {
		typedef typename KDTree<int, double, 2>::Node::Ptr NodePtr;
		typedef typename KDTree<int, double, 2>::AABB      AABB;
		auto offset = tree->d_root->Volume.min() - Eigen::Vector2d(20, 20);
		auto size =
		    tree->d_root->Volume.max() - offset + Eigen::Vector2d(20, 20);
		auto result = std::make_unique<video::Frame>(
		    size.x(),
		    size.y(),
		    AV_PIX_FMT_RGB24,
		    32
		);
		for (size_t i = 0; i < 3; i++) {
			memset(result->Planes[i], 255, result->Linesize[i] * size.y());
		}

		auto drawAABB =
		    [&result](const AABB &volume, const Color &color, int thickness) {
			    drawLine(
			        *result,
			        volume.min(),
			        Eigen::Vector2d(volume.min().x(), volume.max().y()),
			        color,
			        thickness
			    );
			    drawLine(
			        *result,
			        Eigen::Vector2d(volume.min().x(), volume.max().y()),
			        volume.max(),
			        color,
			        thickness
			    );
			    drawLine(
			        *result,
			        volume.max(),
			        Eigen::Vector2d(volume.max().x(), volume.min().y()),
			        color,
			        thickness
			    );
			    drawLine(
			        *result,
			        Eigen::Vector2d(volume.max().x(), volume.min().y()),
			        volume.min(),
			        color,
			        thickness
			    );
		    };

		std::vector<NodePtr> toProcess = {
		    tree->d_root,
		};
		std::vector<AABB> volumes = {
		    AABB(tree->d_root->Volume),
		};
		drawAABB(volumes[0], {0, 0, 0}, 4);
		for (size_t i = 0; i < toProcess.size(); ++i) {
			auto   n      = toProcess[i];
			auto   volume = volumes[i];
			auto   center = (n->ObjectVolume.min() + n->ObjectVolume.max()) / 2;
			size_t dim    = n->Depth % 2;
			if (n->Lower) {
				auto newVolume          = volume;
				newVolume.max()(dim, 0) = center(dim, 0);
				toProcess.push_back(n->Lower);
				volumes.push_back(newVolume);
			}
			if (n->Upper) {
				auto newVolume          = volume;
				newVolume.min()(dim, 0) = center(dim, 0);
				toProcess.push_back(n->Upper);
				volumes.push_back(newVolume);
			}

			drawAABB(n->ObjectVolume, {255, 255, 0}, 2);
			drawAABB(n->Volume, {0, 0, 0}, 1);

			if (dim == 0) {
				drawLine(
				    *result,
				    Eigen::Vector2d(center.x(), volume.min().y()),
				    Eigen::Vector2d(center.x(), volume.max().y()),
				    {0, 0, 255},
				    2
				);
			} else {
				drawLine(
				    *result,
				    Eigen::Vector2d(volume.min().x(), center.y()),
				    Eigen::Vector2d(volume.max().x(), center.y()),
				    {255, 0, 0},
				    2
				);
			}
			/*
			cv::putText(
			    result,
			    std::to_string(n->Object),
			    toCv(center + Eigen::Vector2d(4, 4)),
			    cv::FONT_HERSHEY_SIMPLEX,
			    1.0,
			    cv::Vec3b(0, 0, 0)
			);
			cv::circle(result, toCv(center), 4, cv::Vec3b(0, 0, 0), -1);
			*/
		}
		return result;
	}
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
