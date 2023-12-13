
#include "fort/myrmidon/TestSetup.hpp"
#include "fort/myrmidon/priv/VideoReader.hpp"
#include <gtest/gtest.h>

namespace fort {
namespace myrmidon {
namespace priv {

class VideoReaderUTest : public ::testing::Test {};

TEST_F(VideoReaderUTest, GetBaseStreamInfo) {
	const auto &tddConfig = TestSetup::UTestData().NestDataDirs().front();
	const auto &config    = TestSetup::UTestData().Config();
	ASSERT_TRUE(tddConfig.HasMovie);

	ASSERT_EQ(tddConfig.Segments.size(), 1);
	VideoReader vr(tddConfig.AbsoluteFilePath / "stream.0000.mp4");

	EXPECT_EQ(
	    vr.Duration(),
	    (tddConfig.EndFrame - tddConfig.StartFrame + 1) * config.Framerate
	);

	EXPECT_EQ(vr.Length(), tddConfig.EndFrame - tddConfig.StartFrame + 1);
	EXPECT_EQ(vr.Size(), std::make_tuple(config.Width, config.Height));
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
