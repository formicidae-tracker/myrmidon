
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

TEST_F(VideoReaderUTest, CanGrabAllImages) {
	const auto &tddConfig = TestSetup::UTestData().NestDataDirs().front();
	ASSERT_TRUE(tddConfig.HasMovie);

	ASSERT_EQ(tddConfig.Segments.size(), 1);
	VideoReader vr(tddConfig.AbsoluteFilePath / "stream.0000.mp4");

	ASSERT_EQ(vr.Length(), tddConfig.EndFrame - tddConfig.StartFrame + 1);
	for (size_t i = 0; i < vr.Length(); i++) {
		SCOPED_TRACE(
		    "frame: " + std::to_string(i + 1) + "/" +
		    std::to_string(vr.Length())
		);
		EXPECT_NO_THROW({
			auto frame = vr.Grab();

			EXPECT_EQ(frame->Index(), i);
		});
	}
	EXPECT_FALSE(vr.Grab());
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
