#include "Fakedata.hpp"

#include "GeneratedData.hpp"
#include "HermesFileWriter.hpp"
#include "MovieWriter.hpp"
#include "CloseUpWriter.hpp"
#include "FrameDrawer.hpp"

#include <opencv2/core.hpp>

namespace fort {
namespace myrmidon {

Fakedata::Fakedata(const fs::path & basedir) {
	try {
		BuildFakeData(basedir);
	} catch (const std::exception & e) {
		CleanUpFilesystem();
		throw;
	}
}

Fakedata::~Fakedata() {
	//	CleanUpFilesystem();
}



void Fakedata::CleanUpFilesystem() {
	if (d_basedir.empty()) {
		return;
	}
	fs::remove_all(d_basedir);
};

void Fakedata::BuildFakeData(const fs::path & basedir) {
	auto start = Time::Now();
	d_basedir = basedir;
	GenerateFakedata();
	WriteFakedata();
	std::cerr << "Generated data in " << Time::Now().Sub(start) << std::endl;
}


void Fakedata::GenerateFakedata() {
	GeneratedData gen(d_config);
	SaveFullExpectedResult(gen);
	GenerateTruncatedResults();
	GenerateTDDStructure();
}

void Fakedata::SaveFullExpectedResult(const GeneratedData & gen) {
	ExpectedResult full;
	full.Start = Time::SinceEver();
	full.End = Time::Forever();
	full.MaximumGap = Duration::Hour;
	full.Trajectories = gen.Trajectories;
	full.Interactions = gen.Interactions;
	d_results.push_back(full);
	d_frames = gen.Frames;
}

void Fakedata::GenerateTruncatedResults() {
}


void Fakedata::GenerateTDDStructure() {
	d_nestTDDs =
		{
		 {
		  .AbsoluteFilePath = (d_basedir / "nest.0000").string(),
		  .HasFullFrame = false,
		  .HasMovie = true,
		  .HasConfig = true,
		  .Start = d_config.Start,
		  .End = d_config.Start.Add(20*Duration::Second),
		 },
		 {
		  .AbsoluteFilePath = (d_basedir / "nest.0001").string(),
		  .HasFullFrame = true,
		  .HasMovie = false,
		  .HasConfig = true,
		  .Start = d_config.Start.Add(20*Duration::Second),
		  .End = d_config.Start.Add(3*Duration::Minute),
		 },
		 {
		  .AbsoluteFilePath = (d_basedir / "nest.0002").string(),
		  .HasFullFrame = true,
		  .HasMovie = false,
		  .HasConfig = true,
		  .Start = d_config.Start.Add(3*Duration::Minute),
		  .End = d_config.End,
		 },
		};

		d_forageTDDs =
		{
		 {
		  .AbsoluteFilePath = (d_basedir / "forage.0000").string(),
		  .HasFullFrame = true,
		  .HasMovie = false,
		  .HasConfig = true,
		  .Start = d_config.Start,
		  .End = d_config.End,
		 },
		};
}

void Fakedata::WriteFakedata() {
	WriteTDDs();
}


void Fakedata::WriteTDDs() {
	for ( const auto & tddInfo : d_nestTDDs ) {
		WriteTDD(tddInfo,1);
	}
	for ( const auto & tddInfo : d_forageTDDs ) {
		WriteTDD(tddInfo,2);
	}
}

void Fakedata::WriteTDD(const TDDInfo & tddInfo,SpaceID spaceID) {
	fs::create_directories(tddInfo.AbsoluteFilePath);
	if ( tddInfo.HasConfig ) {
		WriteTDDConfig(tddInfo);
	}
	auto drawer = std::make_shared<FrameDrawer>(d_config);

	SegmentedDataWriter::List writers
		= {
		   std::make_shared<HermesFileWriter>(tddInfo.AbsoluteFilePath,d_config),
		   std::make_shared<CloseUpWriter>(drawer),
	};
	if ( tddInfo.HasMovie ) {
		writers.push_back(std::make_shared<MovieWriter>(drawer));
	}
	WriteSegmentedData(tddInfo,spaceID,writers);

}

void Fakedata::WriteSegmentedData(const TDDInfo & tddInfo,
                                  SpaceID spaceID,
                                  const SegmentedDataWriter::List & writers ) {
	size_t i = 0;
	uint64_t frameID = 0;
	for ( Time current = tddInfo.Start;
	      current.Before(tddInfo.End);
	      current = current.Add(d_config.Segment) ) {
		auto endTime = std::min(current.Add(d_config.Segment),tddInfo.End);
		auto begin = std::find_if(d_frames.begin(),
		                          d_frames.end(),
		                          [&](const std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> & it ) {
			                          return it.first->FrameTime >= current;
		                          });
		auto end = std::find_if(d_frames.begin(),
		                        d_frames.end(),
		                        [&](const std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> & it ) {
			                        return it.first->FrameTime >= endTime;
		                        });
		std::for_each(writers.begin(),writers.end(),
		              [&](const SegmentedDataWriter::Ptr & w) {
			              w->Prepare(i);
		              });
		for(auto iter = begin; iter != end ; ++iter ) {
			if ( iter->first->Space != spaceID ) {
				continue;
			}
			std::for_each(writers.begin(),writers.end(),
			              [&](const SegmentedDataWriter::Ptr & w) {
				             w->WriteFrom(iter->first,++frameID);
			              });
		}
		std::for_each(writers.begin(),writers.end(),
		              [&](const SegmentedDataWriter::Ptr & w) {
			              w->Finalize(i,endTime == tddInfo.End);
		              });
		++i;
	}
}



void Fakedata::WriteTDDConfig(const TDDInfo & info) {
}

} // namespace myrmidon
} // namespace fort
