#include <gtest/gtest.h>

#include "TrackingDataDirectory.hpp"
#include <fort/myrmidon/Experiment.pb.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/util/time_util.h>

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/myrmidon/UtilsUTest.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>
#include <fort/myrmidon/utils/NotYetImplemented.hpp>

#include "RawFrame.hpp"
#include "TagCloseUp.hpp"
#include "UtilsUTest.hpp"

#include <yaml-cpp/yaml.h>

namespace fort {
namespace myrmidon {
namespace priv {

class TrackingDataDirectoryUTest : public ::testing::Test {};

TEST_F(TrackingDataDirectoryUTest, ExtractInfoFromTrackingDatadirectories) {

	try {
		const auto &tddInfo   = TestSetup::UTestData().NestDataDirs().back();
		auto        startOpen = Time::Now();

		auto [tdd, errors] = TrackingDataDirectory::Open(
		    tddInfo.AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
		auto endOpen = Time::Now();
		std::cerr << "Opening " << tddInfo.AbsoluteFilePath << " took "
		          << endOpen.Sub(startOpen) << std::endl;
		EXPECT_EQ(tdd->URI(), tddInfo.AbsoluteFilePath.filename());
		EXPECT_TRUE(errors.empty());
		EXPECT_EQ(tdd->StartFrame(), tddInfo.StartFrame);
		EXPECT_EQ(tdd->EndFrame(), tddInfo.EndFrame);
		EXPECT_TIME_EQ(tdd->Start(), tddInfo.Start);
		EXPECT_TIME_EQ(tdd->End(), tddInfo.End);

		ASSERT_EQ(
		    tddInfo.Segments.size(),
		    tdd->TrackingSegments().Segments().size()
		);

		for (size_t i = 0; i < tddInfo.Segments.size(); ++i) {
			// Can make mistakes about path extraction quite easily
			EXPECT_EQ(
			    tddInfo.Segments[i].URI,
			    tdd->TrackingSegments().Segments()[i].first.URI()
			);
			EXPECT_EQ(
			    tddInfo.Segments[i].FrameID,
			    tdd->TrackingSegments().Segments()[i].first.FrameID()
			) << " With i: "
			  << i;
			EXPECT_TIME_EQ(
			    tddInfo.Segments[i].Start,
			    tdd->TrackingSegments().Segments()[i].first.Time()
			) << " With i: "
			  << i;
			EXPECT_EQ(
			    tddInfo.Segments[i].RelativePath,
			    tdd->TrackingSegments().Segments()[i].second
			) << " With i: "
			  << i;
		}

		uint64_t i         = tdd->StartFrame();
		auto     iterStart = Time::Now();

		for (auto it = tdd->begin(); it != tdd->end(); ++it) {
			auto f = *it;
			EXPECT_EQ(f->Frame().FrameID(), i);
			ASSERT_TRUE(f->Tags().size() >= 2);
			ASSERT_TRUE(f->Tags().size() <= 3);
			EXPECT_EQ(f->Tags().Get(0).id(), 0);
			EXPECT_EQ(f->Tags().Get(1).id(), 1);
			if (f->Tags().size() == 3) {
				EXPECT_EQ(f->Tags().Get(2).id(), 2);
			}
			++i;
		}
		auto iterEnd = Time::Now();
		std::cerr << "Iterating over all frames from "
		          << tddInfo.AbsoluteFilePath << " took "
		          << iterEnd.Sub(iterStart) << std::endl;

	} catch (const std::exception &e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

	auto noDataDir = TestSetup::UTestData().Basedir() / "no-data.0000";
	ASSERT_NO_THROW({ fs::create_directories(noDataDir); });

	EXPECT_THROW(
	    {
		    // no tracking data
		    auto tdd = TrackingDataDirectory::Open(
		        noDataDir,
		        TestSetup::UTestData().Basedir()
		    );
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    // directory does not exists
		    auto tdd = TrackingDataDirectory::Open(
		        TestSetup::UTestData().Basedir() / "foo.1234",
		        TestSetup::UTestData().Basedir()
		    );
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    // is not a directory
		    auto tdd = TrackingDataDirectory::Open(
		        TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath,
		        TestSetup::UTestData().Basedir()
		    );
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    // root does not exist
		    auto tdd = TrackingDataDirectory::Open(
		        TestSetup::UTestData().NestDataDirs().front().AbsoluteFilePath,
		        TestSetup::UTestData().Basedir() / "dir-does-not-exists"
		    );
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    // no configuration
		    auto tdd = TrackingDataDirectory::Open(
		        TestSetup::UTestData().NoConfigDataDir().AbsoluteFilePath,
		        TestSetup::UTestData().Basedir()
		    );
	    },
	    std::runtime_error
	);
}

TEST_F(TrackingDataDirectoryUTest, HasUIDBasedOnPath) {
	struct TestData {
		fs::path A, B;
		bool     Expected;
	};

	std::vector<TestData> data = {
	    {
	        "bar/foo",
	        "bar/foo",
	        true,
	    },
	    {
	        "bar/foo",
	        "bar////foo",
	        true,
	    },
	    {
	        "bar/foo",
	        "baz/foo",
	        false,
	    },
	    {
	        "bar/../foo",
	        "baz/../foo",
	        true,
	    },
	};

	for (const auto &d : data) {
		auto aUID = TrackingDataDirectory::GetUID(d.A);
		auto bUID = TrackingDataDirectory::GetUID(d.B);
		EXPECT_EQ(aUID == bUID, d.Expected);
	}
}

TEST_F(TrackingDataDirectoryUTest, HaveConstructorChecks) {
	uint64_t startFrame = 10;
	uint64_t endFrame   = 20;
	auto     startTime  = Time::Parse("2019-11-02T22:02:24.674+01:00");
	auto     endTime    = Time::Parse("2019-11-02T22:02:25.783+01:00");
	auto segments = std::make_shared<TrackingDataDirectory::TrackingIndex>();
	auto movies   = std::make_shared<TrackingDataDirectory::MovieIndex>();
	auto cache = std::make_shared<TrackingDataDirectory::FrameReferenceCache>();
	auto absolutePath = TestSetup::UTestData().Basedir() / "bar";
	EXPECT_NO_THROW({
		TrackingDataDirectory::Create(
		    "foo",
		    absolutePath,
		    startFrame,
		    endFrame,
		    startTime,
		    endTime,
		    segments,
		    movies,
		    cache
		);
	});

	EXPECT_THROW(
	    {
		    TrackingDataDirectory::Create(
		        "foo",
		        "bar",
		        startFrame,
		        endFrame,
		        startTime,
		        endTime,
		        segments,
		        movies,
		        cache
		    );
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    TrackingDataDirectory::Create(
		        "foo",
		        absolutePath,
		        endFrame,
		        startFrame,
		        startTime,
		        endTime,
		        segments,
		        movies,
		        cache
		    );
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    TrackingDataDirectory::Create(
		        "foo",
		        absolutePath,
		        startFrame,
		        endFrame,
		        endTime,
		        startTime,
		        segments,
		        movies,
		        cache
		    );
	    },
	    std::invalid_argument
	);
}

TEST_F(TrackingDataDirectoryUTest, AlmostRandomAccess) {
	auto tddPath =
	    TestSetup::UTestData().NestDataDirs().back().AbsoluteFilePath;
	auto [tdd, errors] =
	    TrackingDataDirectory::Open(tddPath, TestSetup::UTestData().Basedir());
	EXPECT_TRUE(errors.empty());
	EXPECT_NO_THROW({
		FrameID middle = (tdd->StartFrame() + tdd->EndFrame()) / 2;
		tdd->FrameReferenceAt(middle);
	});

	EXPECT_NO_THROW({
		EXPECT_EQ(tdd->FrameAt(tdd->EndFrame() + 1), tdd->end());
	});

	EXPECT_THROW(
	    { tdd->FrameReferenceAt(tdd->EndFrame() + 1); },
	    std::out_of_range
	);

	EXPECT_NO_THROW({
		auto iter = tdd->FrameAfter(tdd->Start());
		EXPECT_EQ(iter, tdd->begin());
		auto next = tdd->FrameAfter(tdd->Start().Add(1));
		EXPECT_EQ(++iter, next);
		auto iterLast = tdd->FrameAfter(tdd->End().Add(-1));
		EXPECT_EQ((*iterLast)->Frame().FrameID(), tdd->EndFrame());
		auto iterEnd = tdd->FrameAfter(tdd->End());
		EXPECT_EQ(iterEnd, tdd->end());
	});

	EXPECT_THROW(
	    { auto iterEnd = tdd->FrameAfter(tdd->Start().Add(-1)); },
	    std::out_of_range
	);

	EXPECT_THROW({ tdd->FrameReferenceAfter(tdd->End()); }, std::out_of_range);

	EXPECT_NO_THROW({
		auto ref = tdd->FrameReferenceAfter(tdd->Start());
		EXPECT_EQ(ref.FrameID(), tdd->StartFrame());
		ref = tdd->FrameReferenceAfter(tdd->Start().Add(1));
		EXPECT_EQ(ref.FrameID(), tdd->StartFrame() + 1);
	});
}

TEST_F(TrackingDataDirectoryUTest, CanBeFormatted) {
	TrackingDataDirectory::Ptr nest;
	FixableErrorList           errors;
	EXPECT_NO_THROW({
		std::tie(nest, errors) = TrackingDataDirectory::Open(
		    TestSetup::UTestData().Basedir() / "nest.0000",
		    TestSetup::UTestData().Basedir() / "foraging.0000"
		);
	});
	EXPECT_TRUE(errors.empty());
	std::ostringstream oss;
	oss << *nest;
	EXPECT_EQ(
	    oss.str(),
	    "TDD{URI:'../nest.0000', start:" +
	        TestSetup::UTestData().NestDataDirs().front().Start.Format() +
	        ", end:" +
	        TestSetup::UTestData().NestDataDirs().front().End.Format() + "}"
	);
}

::testing::AssertionResult ApriltagOptionsEqual(
    const tags::ApriltagOptions &a, const tags::ApriltagOptions &b
) {
	if (a.Family != b.Family) {
		return ::testing::AssertionFailure()
		       << "a.Family=" << tags::GetFamilyName(a.Family)
		       << " and b.Family=" << tags::GetFamilyName(b.Family)
		       << " differs";
	}
#define MY_ASSERT_FLOAT(fieldName)                                             \
	do {                                                                       \
		auto fieldName##Assertion =                                            \
		    ::testing::internal::CmpHelperFloatingPointEQ<float>(              \
		        "a." #fieldName,                                               \
		        "b." #fieldName,                                               \
		        a.fieldName,                                                   \
		        b.fieldName                                                    \
		    );                                                                 \
		if (fieldName##Assertion == false) {                                   \
			return fieldName##Assertion;                                       \
		}                                                                      \
	} while (0)
#define MY_ASSERT_OTHER(fieldName)                                             \
	do {                                                                       \
		if (a.fieldName != b.fieldName) {                                      \
			return ::testing::AssertionFailure()                               \
			       << "a." << #fieldName << "= " << std::boolalpha             \
			       << a.fieldName << " and b." << #fieldName << "= "           \
			       << std::boolalpha << b.fieldName << " differs";             \
		}                                                                      \
	} while (0)

	MY_ASSERT_FLOAT(QuadDecimate);
	MY_ASSERT_FLOAT(QuadSigma);
	MY_ASSERT_OTHER(RefineEdges);
	MY_ASSERT_OTHER(QuadMinClusterPixel);
	MY_ASSERT_OTHER(QuadMaxNMaxima);
	MY_ASSERT_FLOAT(QuadCriticalRadian);
	MY_ASSERT_FLOAT(QuadMaxLineMSE);
	MY_ASSERT_OTHER(QuadMinBWDiff);
	MY_ASSERT_OTHER(QuadDeglitch);

#undef MY_ASSERT_FLOAT
#undef MY_ASSERT_OTHER
	return ::testing::AssertionSuccess();
}

TEST_F(TrackingDataDirectoryUTest, ParsesDetectionSettings) {
	TrackingDataDirectory::Ptr nest, noFamily;
	FixableErrorList           errors;
	EXPECT_NO_THROW({
		std::tie(nest, errors) = TrackingDataDirectory::Open(
		    TestSetup::UTestData().NestDataDirs().front().AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
	});
	EXPECT_TRUE(errors.empty());
	EXPECT_NO_THROW({
		std::tie(noFamily, errors) = TrackingDataDirectory::Open(
		    TestSetup::UTestData().NoFamilyDataDir().AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
	});
	EXPECT_TRUE(errors.empty());
	tags::ApriltagOptions expected;
	EXPECT_TRUE(ApriltagOptionsEqual(noFamily->DetectionSettings(), expected));

	expected.Family              = tags::Family::Tag36h11;
	expected.QuadMinClusterPixel = 25;
	expected.QuadMinBWDiff       = 75;
	EXPECT_TRUE(ApriltagOptionsEqual(nest->DetectionSettings(), expected));
}

TEST_F(TrackingDataDirectoryUTest, ComputesAndCacheTagStatistics) {
	TrackingDataDirectory::Ptr tdd;
	FixableErrorList           errors;
	auto                       tddPath =
	    TestSetup::UTestData().NestDataDirs().back().AbsoluteFilePath;
	ASSERT_NO_THROW({
		UTestData::ClearCachedData(tddPath);
		std::tie(tdd, errors) = TrackingDataDirectory::Open(
		    tddPath,
		    TestSetup::UTestData().Basedir()
		);
	});
	EXPECT_TRUE(errors.empty());
	EXPECT_FALSE(tdd->TagStatisticsComputed());
	EXPECT_THROW(
	    { tdd->TagStatistics(); },
	    TrackingDataDirectory::ComputedRessourceUnavailable
	);

	TagStatisticsHelper::Timed computedStats, cachedStats;
	try {
		auto loaders = tdd->PrepareTagStatisticsLoaders();
		EXPECT_EQ(loaders.size(), 2);
		for (const auto &l : loaders) {
			l();
		}

		EXPECT_TRUE(tdd->TagStatisticsComputed());
		computedStats = tdd->TagStatistics();
	} catch (const std::exception &e) {
		ADD_FAILURE() << "Computation should not throw this excption: "
		              << e.what();
	}

	tdd.reset();
	ASSERT_NO_THROW({
		std::tie(tdd, errors) = TrackingDataDirectory::Open(
		    tddPath,
		    TestSetup::UTestData().Basedir()
		);
		cachedStats = tdd->TagStatistics();
	});
	EXPECT_TRUE(errors.empty());
	EXPECT_TRUE(tdd->TagStatisticsComputed());
	EXPECT_PRED_FORMAT2(AssertTimedStatsEqual, cachedStats, computedStats);
}

TEST_F(TrackingDataDirectoryUTest, ComputesAndCacheFullFrames) {
	auto tddPath = TestSetup::UTestData().WithVideoDataDir().AbsoluteFilePath;
	TrackingDataDirectory::Ptr tdd;
	FixableErrorList           errors;
	ASSERT_NO_THROW({
		UTestData::ClearCachedData(tddPath);
		std::tie(tdd, errors) = TrackingDataDirectory::Open(
		    tddPath,
		    TestSetup::UTestData().Basedir()
		);
	});
	EXPECT_TRUE(errors.empty());
	EXPECT_FALSE(tdd->FullFramesComputed());
	EXPECT_THROW(
	    { tdd->FullFrames(); },
	    TrackingDataDirectory::ComputedRessourceUnavailable
	);

	try {
		auto loaders = tdd->PrepareFullFramesLoaders();
		EXPECT_EQ(loaders.size(), 1);
		for (const auto &l : loaders) {
			l();
		}
		EXPECT_EQ(tdd->FullFrames().size(), 1);
	} catch (const std::exception &e) {
		ADD_FAILURE() << "Computation should not throw this exception: "
		              << e.what();
	}

	EXPECT_TRUE(tdd->FullFramesComputed());
	ASSERT_NO_THROW({
		std::tie(tdd, errors) = TrackingDataDirectory::Open(
		    tddPath,
		    TestSetup::UTestData().Basedir()
		);
		EXPECT_EQ(tdd->FullFrames().size(), 1);
	});
	EXPECT_TRUE(errors.empty());
	EXPECT_TRUE(tdd->FullFramesComputed());
}

TEST_F(TrackingDataDirectoryUTest, CanListTagCloseUpFiles) {
	for (const auto &tddInfo : TestSetup::UTestData().NestDataDirs()) {
		UTestData::ClearCachedData(tddInfo.AbsoluteFilePath);
		auto files = TrackingDataDirectory::ListTagCloseUpFiles(
		    tddInfo.AbsoluteFilePath / "ants"
		);
		auto expectedFiles = tddInfo.TagCloseUpFiles;
		ASSERT_EQ(files.size(), expectedFiles.size());
		for (const auto &[frameID, ff] : expectedFiles) {
			const auto &[fi, end] = files.equal_range(frameID);
			if (fi == end) {
				ADD_FAILURE()
				    << "Returned a file for unexpected frameID " << frameID;
			} else {
				auto ffi = std::find_if(fi, end, [&ff](const auto &it) {
					return ff.first == it.second.first;
				});
				if (ffi == end) {
					ADD_FAILURE()
					    << "Missing file " << ff.first.generic_string();
				} else if (!(ff.second) != !(ffi->second.second)) {
					ADD_FAILURE() << "Filtering mismatch for file "
					              << ff.first.generic_string();
				} else if (ff.second) {
					EXPECT_EQ(*ff.second, *ffi->second.second);
				}
			}
		}
	}
}

TEST_F(TrackingDataDirectoryUTest, ComputesAndCacheTagCloseUps) {
	const auto &tddInfo = TestSetup::UTestData().NestDataDirs().back();
	auto        tddPath = tddInfo.AbsoluteFilePath;
	TrackingDataDirectory::Ptr tdd;
	FixableErrorList           errors;
	ASSERT_NO_THROW({
		UTestData::ClearCachedData(tddPath);
		std::tie(tdd, errors) = TrackingDataDirectory::Open(
		    tddPath,
		    TestSetup::UTestData().Basedir()
		);
	});
	EXPECT_TRUE(errors.empty());
	EXPECT_FALSE(tdd->TagCloseUpsComputed());
	EXPECT_THROW(
	    { tdd->TagCloseUps(); },
	    TrackingDataDirectory::ComputedRessourceUnavailable
	);

	std::vector<TagCloseUp::ConstPtr> computed, cached;

	try {
		auto loaders = tdd->PrepareTagCloseUpsLoaders();
		for (const auto &l : loaders) {
			l();
		}
	} catch (const std::exception &e) {
		ADD_FAILURE() << "Computation should not throw this exception: "
		              << e.what();
	}

	EXPECT_TRUE(tdd->TagCloseUpsComputed());

	ASSERT_NO_THROW({
		computed              = tdd->TagCloseUps();
		std::tie(tdd, errors) = TrackingDataDirectory::Open(
		    tddPath,
		    TestSetup::UTestData().Basedir()
		);
		ASSERT_FALSE(tdd->TagCloseUps().empty());
		cached = tdd->TagCloseUps();
	});
	EXPECT_TRUE(errors.empty());
	EXPECT_TRUE(tdd->TagCloseUpsComputed());

	ASSERT_EQ(cached.size(), tddInfo.TagCloseUps.size());
	ASSERT_EQ(computed.size(), tddInfo.TagCloseUps.size());

	std::sort(
	    cached.begin(),
	    cached.end(),
	    [](const priv::TagCloseUp::ConstPtr &a,
	       const priv::TagCloseUp::ConstPtr &b) {
		    if (a->AbsoluteFilePath() == b->AbsoluteFilePath()) {
			    return a->TagValue() < b->TagValue();
		    }
		    return a->AbsoluteFilePath() < b->AbsoluteFilePath();
	    }
	);

	std::sort(
	    computed.begin(),
	    computed.end(),
	    [](const priv::TagCloseUp::ConstPtr &a,
	       const priv::TagCloseUp::ConstPtr &b) {
		    if (a->AbsoluteFilePath() == b->AbsoluteFilePath()) {
			    return a->TagValue() < b->TagValue();
		    }
		    return a->AbsoluteFilePath() < b->AbsoluteFilePath();
	    }
	);

	auto expectTCUEq = [](const priv::TagCloseUp::ConstPtr &result,
	                      const priv::TagCloseUp::ConstPtr &expected) {
		EXPECT_EQ(result->AbsoluteFilePath(), expected->AbsoluteFilePath())
		    << " for " << expected->URI();
		EXPECT_EQ(result->TagValue(), expected->TagValue())
		    << " for " << expected->URI();

		EXPECT_TRUE(
		    (result->TagPosition() - expected->TagPosition()).squaredNorm() <
		    9.0
		) << " for "
		  << expected->URI() << " - " << expected->AbsoluteFilePath()
		  << std::endl
		  << " got:    " << result->TagPosition().transpose() << std::endl
		  << " expect: " << expected->TagPosition().transpose();

		for (int i = 0; i < 4; ++i) {
			EXPECT_TRUE(
			    (result->Corners()[i] - expected->Corners()[i]).squaredNorm() <
			    9.0
			) << " for "
			  << expected->URI() << " - " << expected->AbsoluteFilePath()
			  << " corner " << i << std::endl
			  << " got:    " << result->Corners()[i].transpose() << std::endl
			  << " expect: " << expected->Corners()[i].transpose();
		}

		EXPECT_TRUE(
		    std::abs(result->TagAngle() - expected->TagAngle()) <
		    5 * M_PI / 180.0
		) << " for "
		  << expected->URI();
	};

	for (const auto &tcu : tddInfo.TagCloseUps) {
		auto co = std::find_if(
		    computed.begin(),
		    computed.end(),
		    [&tcu](const priv::TagCloseUp::ConstPtr &itcu) {
			    return itcu->URI() == tcu->URI() &&
			           tcu->AbsoluteFilePath() == itcu->AbsoluteFilePath();
		    }
		);
		if (co == computed.end()) {
			ADD_FAILURE() << "missing computed close-up " << tcu->URI();
			continue;
		}
		auto ca = std::find_if(
		    cached.begin(),
		    cached.end(),
		    [&tcu](const priv::TagCloseUp::ConstPtr &itcu) {
			    return itcu->URI() == tcu->URI() &&
			           tcu->AbsoluteFilePath() == itcu->AbsoluteFilePath();
		    }
		);
		if (ca == cached.end()) {
			ADD_FAILURE() << "missing cached close-up " << tcu->URI();
			continue;
		}
		expectTCUEq(*co, tcu);
		expectTCUEq(*ca, tcu);
	}
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
