#pragma once

#include <functional>

#include <fort/myrmidon/Types.hpp>
#include <fort/myrmidon/Query.hpp>

#include "Matchers.hpp"
#include <set>

namespace fort {
namespace myrmidon {
namespace priv {

class DataSegmenter {
public:
	struct Args {
		std::function<void (AntTrajectory::Ptr &)> StoreTrajectory;
		std::function<void (AntInteraction::Ptr &)> StoreInteraction;

		Duration            MaximumGap;
		priv::Matcher::Ptr  Matcher;
		bool                SummarizeSegment;
	};

	DataSegmenter(const Args & args);
	~DataSegmenter();

	void operator()(const myrmidon::Query::CollisionData & data);
private:
	struct BuildingInteraction;
	struct BuildingTrajectory {
		typedef std::shared_ptr<BuildingTrajectory> Ptr;
		std::shared_ptr<AntTrajectory> Trajectory;

		Time                  Last;
		std::vector<double>   DataPoints;
		bool                  ForceKeep;

		std::set<BuildingInteraction*> Interactions;

		BuildingTrajectory(const IdentifiedFrame & frame,
		                   const PositionedAntConstRef & ant);

		void Append(const IdentifiedFrame & frame,
		            const PositionedAntConstRef & ant);

		size_t Size() const;

		Time TimeAt(size_t index) const;

		size_t FindIndexFor(const Time & time,
		                    size_t low,
		                    size_t high);

		AntTrajectory::Ptr Terminate();
	};


	struct BuildingInteraction {
		typedef std::unique_ptr<BuildingInteraction> Ptr;
		InteractionID IDs;
		Time          Start,Last;

		std::pair<size_t,size_t>                                   SegmentStarts,MinEnd,MaxEnd;
		std::pair<BuildingTrajectory::Ptr,BuildingTrajectory::Ptr> Trajectories;

		std::set<std::pair<AntShapeTypeID,AntShapeTypeID>> Types;

		~BuildingInteraction();

		BuildingInteraction(const Collision & collision,
		                    const Time & curTime,
							std::pair<BuildingTrajectory::Ptr,BuildingTrajectory::Ptr> trajectories);

		void Append(const Collision & collision,
		            const Time & curTime);


		static void SummarizeTrajectorySegment(AntTrajectorySegment & s);

		AntInteraction::Ptr Terminate(bool summarize);
	};


	void BuildTrajectories(const IdentifiedFrame::Ptr & identified,
						   bool conserveAllTrajectory);

	void TerminateTrajectory(const BuildingTrajectory::Ptr & trajectory);

	void BuildInteractions(const CollisionFrame::Ptr & collisions);



	std::map<AntID,BuildingTrajectory::Ptr>     d_trajectories;
	std::map<InteractionID,BuildingInteraction::Ptr> d_interactions;
	Args                                        d_args;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort