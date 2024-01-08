#include <gtest/gtest.h>

#include <fort/myrmidon/ExperimentFile.pb.h>
#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/priv/proto/FileReadWriter.hpp>

#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

class FileReadWriterUTest : public ::testing::Test {};

TEST_F(FileReadWriterUTest, TestBadIO) {
	typedef FileReadWriter<pb::FileHeader, pb::FileLine> RW;

	pb::FileHeader h;

	EXPECT_THROW(
	    {
		    RW::Write(
		        TestSetup::UTestData().Basedir() / "does-no-exist-dir" /
		            "foo.myrmidon",
		        h,
		        {}
		    );
	    },
	    std::runtime_error
	);

	EXPECT_THROW(
	    {
		    RW::Read(
		        TestSetup::UTestData().Basedir() / "does-no-exist-dir" /
		            "foo.myrmidon",
		        [](const pb::FileHeader &h) {},
		        [](const pb::FileLine &line) {}
		    );
	    },
	    std::runtime_error
	);
}

} // namespace proto
} // namespace priv
} // namespace myrmidon
} // namespace fort
