#include "fort/myrmidon/types/Collision.hpp"
#include <gtest/gtest.h>

#include <fort/myrmidon/types/AntInteraction.hpp>

namespace fort {
namespace myrmidon {
class AntInteractionTest : public ::testing::Test {};

TEST_F(AntInteractionTest, CanReportTypeTest) {
	InteractionTypes types(3, 2);
	types << 1, 1, 1, 2, 2, 2;

	auto interaction = AntInteraction{
	    .Types = types,
	    .Start = Time(),
	    .End   = Time(),
	};

	EXPECT_TRUE(interaction.HasInteractionType(1, 1));
	EXPECT_TRUE(interaction.HasInteractionType(1, 2));
	EXPECT_FALSE(interaction.HasInteractionType(2, 1));
	EXPECT_TRUE(interaction.HasInteractionType(2, 2));
	EXPECT_FALSE(interaction.HasInteractionType(3, 1));
}
} // namespace myrmidon
} // namespace fort
