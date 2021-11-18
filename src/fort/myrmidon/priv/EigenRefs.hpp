#pragma once

#include <Eigen/Core>

#include <fort/myrmidon/types/IdentifiedFrame.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


typedef Eigen::Ref<Eigen::Vector2d> Vector2dRef;

typedef Eigen::Ref<IdentifiedFrame::PositionMatrix>       PositionedAntRef;
typedef Eigen::Ref<const IdentifiedFrame::PositionMatrix> PositionedAntConstRef;


} // namespace priv
} // namespace myrmidon
} // namespace fort
