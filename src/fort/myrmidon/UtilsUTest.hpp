#pragma once


#include <Eigen/Core>

#include <google/protobuf/message.h>

#include <fort/myrmidon/types/Value.hpp>
#include <fort/myrmidon/types/TagStatistics.hpp>


#include <gtest/gtest.h>

namespace fort {
class Time;
namespace myrmidon {
class Shape;
class Capsule;
class Circle;
class Polygon;
class IdentifiedFrame;
class CollisionFrame;
class AntTrajectory;
class AntInteraction;
} // namespace myrmidon
} // namespace fort

::testing::AssertionResult AssertTimeEqual(const char * aExpr,
                                           const char * bExpr,
                                           const fort::Time & a,
                                           const fort::Time & b);

#define EXPECT_TIME_EQ(a,b) EXPECT_PRED_FORMAT2(AssertTimeEqual,a,b)

::testing::AssertionResult AssertVectorAlmostEqual(const char * aExpr,
                                                   const char * bExpr,
                                                   const Eigen::Vector2d & a,
                                                   const Eigen::Vector2d & b);

#define EXPECT_VECTOR2D_EQ(a,b) EXPECT_PRED_FORMAT2(AssertVectorAlmostEqual,a,b)

::testing::AssertionResult AssertMessageEqual(const char * aExpr,
                                              const char * bExpr,
                                              const google::protobuf::Message &a,
                                              const google::protobuf::Message &b);

#define EXPECT_MESSAGE_EQ(a,b) EXPECT_PRED_FORMAT2(AssertMessageEqual,a,b)

::testing::AssertionResult AssertPolygonEqual(const char * aExpr,
                                              const char * bExpr,
                                              const fort::myrmidon::Polygon &a,
                                              const fort::myrmidon::Polygon &b);

#define EXPECT_POLYGON_EQ(a,b) EXPECT_PRED_FORMAT2(AssertPolygonEqual,a,b)

::testing::AssertionResult AssertCapsuleEqual(const char * aExpr,
                                              const char * bExpr,
                                              const fort::myrmidon::Capsule &a,
                                              const fort::myrmidon::Capsule &b);

#define EXPECT_CAPSULE_EQ(a,b) EXPECT_PRED_FORMAT2(AssertCapsuleEqual,a,b)


::testing::AssertionResult AssertCircleEqual(const char * aExpr,
                                             const char * bExpr,
                                             const fort::myrmidon::Circle &a,
                                             const fort::myrmidon::Circle &b);

#define EXPECT_CIRCLE_EQ(a,b) EXPECT_PRED_FORMAT2(AssertCircleEqual,a,b)

::testing::AssertionResult AssertShapeEqual(const char * aExpr,
                                            const char * bExpr,
                                            const fort::myrmidon::Shape &a,
                                            const fort::myrmidon::Shape &b);

#define EXPECT_SHAPE_EQ(a,b) EXPECT_PRED_FORMAT2(AssertShapeEqual,a,b)


::testing::AssertionResult AssertValueEqual(const char * aExpr,
                                               const char * bExpr,
                                               const fort::myrmidon::Value &a,
                                               const fort::myrmidon::Value &b);

#define EXPECT_VALUE_EQ(a,b) EXPECT_PRED_FORMAT2(AssertValueEqual,a,b)

::testing::AssertionResult AssertAABBAlmostEqual(const char * aExpr,
                                                 const char * bExpr,
                                                 const fort::myrmidon::AABB & a,
                                                 const fort::myrmidon::AABB & B);

#define EXPECT_AABB_EQ(a,b) EXPECT_PRED_FORMAT2(AssertAABBAlmostEqual,a,b)

::testing::AssertionResult AssertTagStatisticsEqual(const char * aExpr,
                                                    const char * bExpr,
                                                    const fort::myrmidon::TagStatistics::ByTagID & a,
                                                    const fort::myrmidon::TagStatistics::ByTagID & b);

#define EXPECT_TAG_STATISTICS_EQ(a,b) EXPECT_PRED_FORMAT2(AssertTagStatisticsEqual,a,b)


::testing::AssertionResult AssertIdentifiedFrameEqual(const char * aExpr,
                                                      const char * bExpr,
                                                      const fort::myrmidon::IdentifiedFrame & a,
                                                      const fort::myrmidon::IdentifiedFrame & b);

#define EXPECT_IDENTIFIED_FRAME_EQ(a,b) EXPECT_PRED_FORMAT2(AssertIdentifiedFrameEqual,a,b)

::testing::AssertionResult AssertCollisionFrameEqual(const char * aExpr,
                                                      const char * bExpr,
                                                      const fort::myrmidon::CollisionFrame & a,
                                                      const fort::myrmidon::CollisionFrame & b);

#define EXPECT_COLLISION_FRAME_EQ(a,b) EXPECT_PRED_FORMAT2(AssertCollisionFrameEqual,a,b)


::testing::AssertionResult AssertAntTrajectoryEqual(const char * aExpr,
                                                    const char * bExpr,
                                                    const fort::myrmidon::AntTrajectory & a,
                                                    const fort::myrmidon::AntTrajectory & b);

#define EXPECT_ANT_TRAJECTORY_EQ(a,b) EXPECT_PRED_FORMAT2(AssertAntTrajectoryEqual,a,b)


::testing::AssertionResult AssertAntInteractionEqual(const char * aExpr,
                                                     const char * bExpr,
                                                     const fort::myrmidon::AntInteraction & a,
                                                     const fort::myrmidon::AntInteraction & b);

#define EXPECT_ANT_INTERACTION_EQ(a,b) EXPECT_PRED_FORMAT2(AssertAntInteractionEqual,a,b)
