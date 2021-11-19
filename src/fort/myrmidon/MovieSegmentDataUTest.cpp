#include <gtest/gtest.h>

#include "Experiment.hpp"
#include "Query.hpp"

#include "MovieSegmentData.hpp"

#include "TestSetup.hpp"


namespace fort {
namespace myrmidon {

class MovieSegmentDataUTest : public ::testing::Test {
protected:
	void SetUp() {
		experiment = Experiment::Open(TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath);
	}

	Experiment::Ptr experiment;
};


TEST_F(MovieSegmentDataUTest,EndToEnd) {
	const auto & expected = TestSetup::UTestData().ExpectedResults().front();
	const auto & frames = TestSetup::UTestData().ExpectedFrames();
	auto segments = Query::FindMovieSegment(*experiment,
	                                        1,
	                                        expected.Start,
	                                        expected.End);

	MovieSegmentData::MatchData(segments,
	                            frames.begin(),
	                            frames.end());

	MovieSegmentData::MatchData(segments,
	                            expected.Trajectories.begin(),
	                            expected.Trajectories.end());

	MovieSegmentData::MatchData(segments,
	                            expected.Interactions.begin(),
	                            expected.Interactions.end());


}


} // namespace myrmidon
} // namespace fort
