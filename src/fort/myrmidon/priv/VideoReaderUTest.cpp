
#include "fort/myrmidon/TestSetup.hpp"
#include "fort/myrmidon/priv/VideoReader.hpp"
#include <gtest/gtest.h>

namespace fort {
namespace myrmidon {
namespace priv {

class VideoReaderUTest : public ::testing::Test {};

TEST_F(VideoReaderUTest, HelloWorld) {
	const auto &config = TestSetup::UTestData().NestDataDirs().front();

	ASSERT_TRUE(config.HasMovie);

	VideoReader vr(config.AbsoluteFilePath / "stream.0000.mp4");

	EXPECT_EQ(vr.Length(), config.EndFrame - config.StartFrame + 1);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
