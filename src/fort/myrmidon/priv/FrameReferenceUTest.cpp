#include <gtest/gtest.h>

#include "FrameReference.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class FrameReferenceUTest : public ::testing::Test {};


TEST_F(FrameReferenceUTest,CanBeFormatted) {

	struct TestData {
		fs::path    Path;
		fs::path    ExpectedParentPath;
		uint64_t    FrameID;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   {"","/",0,"/frames/0"},
		   {"","/",2134,"/frames/2134"},
		   {"foo","foo",42,"foo/frames/42"},
		   {"foo/bar/baz","foo/bar/baz",42,"foo/bar/baz/frames/42"},
	};


	if (fs::path::preferred_separator == '\\') {
		data.push_back({"foo\bar\baz","foo/bar/baz",42,"foo/bar/baz/frames/42"});
	}


	for(const auto & d : data ) {
		fort::myrmidon::priv::FrameReference a(d.Path.generic_string(),
		                                       d.FrameID,
		                                       fort::Time::FromTimeT(0));
		std::ostringstream os;
		os << a;

		EXPECT_EQ(os.str(),d.Expected);

		EXPECT_EQ(a.ParentURI(),d.ExpectedParentPath);
		auto expectedURI = d.ExpectedParentPath / "frames" / std::to_string(d.FrameID);
		EXPECT_EQ(a.URI(), expectedURI);
	}

}


TEST_F(FrameReferenceUTest,HaveWeakOrdering){
	struct TestData {
		FrameReference A,B;
		bool           Expected;
	};

	std::vector<TestData> testdata =
		{
		 {FrameReference("foo",1,Time()),FrameReference("foo",1,Time()),false},
		 {FrameReference("foo",1,Time()),FrameReference("foo",2,Time()),true},
		 {FrameReference("foo",2,Time()),FrameReference("foo",1,Time()),false},
		 {FrameReference("foo",1,Time()),FrameReference("bar",2,Time()),false},
		};

	for ( const auto & d : testdata ) {
		EXPECT_EQ(d.A < d.B, d.Expected);
	}
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
