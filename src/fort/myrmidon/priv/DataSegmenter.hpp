#pragma once

#include <set>
#include <functional>
#include <vector>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/Query.hpp>

#include <fort/myrmidon/types/AntTrajectory.hpp>
#include <fort/myrmidon/types/AntInteraction.hpp>
#include <fort/myrmidon/types/Collision.hpp>

#include "Matchers.hpp"
#include "EigenRefs.hpp"

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
		bool                SegmentOnMatcherValueChange;
	};

	DataSegmenter(const Args & args);
	~DataSegmenter();

	static AntTrajectorySummary SummarizeTrajectorySegment(AntTrajectorySegment & s);

	void operator()(const myrmidon::CollisionData & data);
private:
	struct BuildingInteraction;
	struct BuildingTrajectory {
		typedef std::shared_ptr<BuildingTrajectory> Ptr;
		std::shared_ptr<AntTrajectory> Trajectory;

		Time                  Last;
		uint64_t              LastValue;
		std::vector<double>   DataPoints;
		bool                  ForceKeep;

		std::set<BuildingInteraction*> Interactions;

		BuildingTrajectory(const IdentifiedFrame & frame,
		                   const PositionedAntConstRef & ant,
		                   uint64_t currentValue);

		void Append(const IdentifiedFrame & frame,
		            const PositionedAntConstRef & ant);

		size_t Size() const;

		Time TimeAt(size_t index) const;

		size_t FindIndexFor(const Time & time,
		                    size_t low,
		                    size_t high);

		bool Matches(const Matcher & m);

		inline Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,5,Eigen::RowMajor>> Mapped() const {
			return Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,5,Eigen::RowMajor>>(&DataPoints[0],Size(),5);
		}

		AntTrajectory::Ptr Terminate();
	};


	struct BuildingInteraction {
		typedef std::unique_ptr<BuildingInteraction> Ptr;
		InteractionID IDs;
		Time          Start,Last;
		uint64_t      LastValue;


		std::pair<size_t,size_t>                                   SegmentStarts,MinEnd,MaxEnd;
		std::pair<BuildingTrajectory::Ptr,BuildingTrajectory::Ptr> Trajectories;

		std::set<std::pair<AntShapeTypeID,AntShapeTypeID>> Types;

		~BuildingInteraction();

		BuildingInteraction(const Collision & collision,
		                    const Time & curTime,
		                    std::pair<BuildingTrajectory::Ptr,BuildingTrajectory::Ptr> trajectories,
		                    uint64_t currentValue);

		void Append(const Collision & collision,
		            const Time & curTime);

		static AntTrajectorySummary
		SummarizeBuildingTrajectory(BuildingTrajectory & trajectory,
		                            size_t begin,
		                            size_t end);

		bool Matches(const Matcher & m);

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
