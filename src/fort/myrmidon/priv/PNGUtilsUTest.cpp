#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/priv/PNGUtils.hpp>
#include <fort/myrmidon/priv/TagCloseUp.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/utils/Defer.hpp>
#include <gtest/gtest.h>
#include <string>

namespace fort {
namespace myrmidon {
namespace priv {

class PNGUtilsUTest : public ::testing::Test {};

TEST_F(PNGUtilsUTest, ReadIO) {
	const auto &config = TestSetup::UTestData().CorruptedDataDir();

	EXPECT_NO_THROW({ ReadPNG(config.TagCloseUps[0]->AbsoluteFilePath()); });
};

TEST_F(PNGUtilsUTest, WriteReadIOPingPong) {
	constexpr static size_t WIDTH  = 16;
	constexpr static size_t HEIGHT = 8;
	auto path = TestSetup::UTestData().Basedir() / "test-rw-ping-png.png";

	auto img = image_u8_create_stride(WIDTH, HEIGHT, WIDTH);
	defer {
		image_u8_destroy(img);
	};

	for (size_t i = 0; i < WIDTH * HEIGHT; i++) {
		img->buf[i] = i;
	}
	EXPECT_NO_THROW({ WritePNG(path, *img); });

	ImageU8Ptr res = ImageU8Ptr{nullptr, nullptr};

	EXPECT_NO_THROW({ res = ReadPNG(path); });

	ASSERT_TRUE(res != nullptr);
	ASSERT_EQ(res->width, WIDTH);
	ASSERT_EQ(res->height, HEIGHT);

	for (size_t y = 0; y < res->height; ++y) {
		for (size_t x = 0; x < res->width; ++x) {
			size_t idx = x + y * res->stride;
			SCOPED_TRACE(
			    "x: " + std::to_string(x) + " y: " + std::to_string(y) +
			    " idx: " + std::to_string(idx)
			);

			EXPECT_EQ(res->buf[idx], WIDTH * y + x);
		}
	}
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
