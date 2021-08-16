#include "TDDCacheUTest.hpp"

#include "TDDCache.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

#include "IOUtils.hpp"

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {


TEST_F(TDDCacheUTest,CacheIO) {

	fs::path cacheURI = TestSetup::UTestData().NestDataDirs().front().AbsoluteFilePath;
	fs::path basedir = TestSetup::UTestData().Basedir();
	EXPECT_THROW({
			// Should be an absolute path as first argument
			TDDCache::Load("nest.0000","nest.0000");
		},std::invalid_argument);

	UTestData::ClearCachedData(cacheURI);

	EXPECT_THROW({
			//Was never opened, so there is no cache
			TDDCache::Load(cacheURI ,basedir);
		},std::runtime_error);

	TrackingDataDirectory::Ptr opened,cached;
	ASSERT_NO_THROW({
			//will open it one first, and saving the cache
			opened = TrackingDataDirectory::Open(cacheURI,
			                                     basedir);
			cached = TDDCache::Load(cacheURI, basedir);
		});


	//Craft a special cache to throw an exception
	pb::TrackingDataDirectory h;
	IOUtils::SaveFrameReference(h.mutable_start(),
	                            FrameReference(cached->URI(),
	                                           cached->StartFrame(),
	                                           cached->Start()));
	IOUtils::SaveFrameReference(h.mutable_end(),
	                            FrameReference(cached->URI(),
	                                           cached->EndFrame(),
	                                           cached->End()));
	std::vector<TDDCache::ReadWriter::LineWriter> lines=
		{
		 [&cached](pb::TrackingDataDirectoryFileLine & line) {

			 IOUtils::SaveMovieSegment(line.mutable_movie(),
			                           cached->MovieSegments().Segments().begin()->second,
			                           cached->AbsoluteFilePath());
		 },
		};
	auto cacheFilepath = cacheURI / TDDCache::CACHE_FILENAME;
	ASSERT_NO_THROW({
			TDDCache::ReadWriter::Write(cacheFilepath,
			                            h,
			                            lines);
		});

	EXPECT_THROW({
			TDDCache::Load(cacheURI, basedir);
		},std::runtime_error);


	ASSERT_NO_THROW({
			fs::remove_all(cacheFilepath);
		});



}


} // namespace proto
} // namespace priv
} // namespace myrmidon
} // namespace fort
