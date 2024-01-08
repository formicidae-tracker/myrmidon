#include "GeneratedData.hpp"

#include "Config.hpp"

#include <fort/time/Time.hpp>
#include <random>

#include <fort/myrmidon/priv/TagStatistics.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

namespace fort {
namespace myrmidon {

std::vector<Time> GeneratedData::DrawFrameTicks(const Config &config) {
	std::random_device rd{};
	std::mt19937       gen{rd()};

	auto period =
	    (Duration::Second * config.Framerate.Den) / config.Framerate.Num;

	std::normal_distribution<> d{0.0, period.Microseconds() * config.Jitter};
	std::uniform_real_distribution<float> u(0, 1);
	std::vector<Time>                     res;
	Duration                              increment;
	for (Time current = config.Start; current.Before(config.End);
	     current      = current.Add(increment)) {
		increment = period + std::clamp(int64_t(d(gen)), -15000L, 15000L) *
		                         Duration::Microsecond;
		if (current > config.Start && u(gen) < 0.05) { // 5% uniform framedrop
			continue;
		}
		res.push_back(current.Round(Duration::Microsecond));
	}
	return res;
}

GeneratedData::GeneratedData(const Config &config, const fs::path &path) {
	GenerateFrameTicks(config, path);
	GenerateTrajectories(config);
	GenerateInteractions(config);
	GenerateFrames(config);
	GenerateTagStatistics(config);
}

void CheckFrameDrop(const std::vector<Time> &ticks, Duration framerate) {
	framerate = 1.5 * framerate.Nanoseconds();
	Time last = ticks.front();
	auto fi   = std::find_if(ticks.begin(), ticks.end(), [&](const Time &t) {
        if (t.Sub(last) > framerate) {
            return true;
        }
        last = t;
        return false;
    });
	if (fi == ticks.end()) {
		throw std::runtime_error("No framedrop found");
	}
}

Duration RoundDuration(Duration v, Duration r) {
	return (v.Nanoseconds() / r.Nanoseconds()) * r;
}

void DrawHistogram(const std::vector<Time> &time) {
	std::map<Duration, int> hist;
	Duration                round = 125 * Duration::Millisecond;
	for (auto it = time.begin() + 1; it != time.end(); ++it) {
		auto d = RoundDuration(it->Sub(*(it - 1)), round);
		++hist[d];
	}
	std::cerr << "histogram of ticks" << std::endl;
	for (const auto &[b, c] : hist) {
		std::cerr << "+ " << b << " - " << (b + round) << ": "
		          << std::string(c / 20, '*') << "(" << c << ") " << std::endl;
	}
	std::cerr << std::endl;
}

void GeneratedData::AssignTicks(
    const std::vector<Time>    &ticks,
    SpaceID                     spaceID,
    const std::vector<TDDData> &TDDs,
    const fs::path             &basepath
) {
	auto current = TDDs.begin();
	auto monoID  = priv::TrackingDataDirectory::GetUID(
        basepath / current->RelativeFilePath
    );
	for (const auto &t : ticks) {
		Ticks.push_back({spaceID, t});
		if (t > current->End) {
			++current;
			if (current != TDDs.end()) {
				monoID = priv::TrackingDataDirectory::GetUID(
				    basepath / current->RelativeFilePath
				);
			}
		}
		if (current != TDDs.end()) {
			auto monoValue =
			    Ticks.back().second.Sub(current->Start).Nanoseconds() + 20e9;
			Ticks.back().second = Time::FromTimestampAndMonotonic(
			    Ticks.back().second.ToTimestamp(),
			    monoValue,
			    monoID
			);
		}
	}
}

void GeneratedData::GenerateFrameTicks(
    const Config &config, const fs::path &basepath
) {
	NestTicks     = DrawFrameTicks(config);
	ForagingTicks = DrawFrameTicks(config);

#ifndef NDEBUG
	DrawHistogram(NestTicks);
	DrawHistogram(ForagingTicks);
#endif

	auto period =
	    (Duration::Second * config.Framerate.Den) / config.Framerate.Num;

	CheckFrameDrop(NestTicks, period);
	CheckFrameDrop(ForagingTicks, period);

	Ticks.clear();
	Ticks.reserve(NestTicks.size() + ForagingTicks.size());

	AssignTicks(NestTicks, 1, config.NestTDDs, basepath);
	AssignTicks(ForagingTicks, 2, config.ForagingTDDs, basepath);

	std::sort(
	    Ticks.begin(),
	    Ticks.end(),
	    [](const std::pair<SpaceID, Time> &a,
	       const std::pair<SpaceID, Time> &b) -> bool {
		    if (a.second == b.second) {
			    return a.first < b.first;
		    }
		    return a.second < b.second;
	    }
	);
}

void GeneratedData::GenerateTrajectories(const Config &config) {
	Trajectories.clear();
	for (const auto &[antID, ant] : config.Ants) {
		GenerateTrajectoriesFor(antID, ant);
	}
	std::sort(
	    Trajectories.begin(),
	    Trajectories.end(),
	    [](const AntTrajectory::Ptr &a, const AntTrajectory::Ptr &b) -> bool {
		    auto aEnd = a->End();
		    auto bEnd = b->End();
		    if (aEnd == bEnd) {
			    return a->Ant < b->Ant;
		    }
		    return aEnd < bEnd;
	    }
	);
#ifndef NDEBUG
	for (const auto &t : Trajectories) {
		std::cerr << "AntTrajectory{ Ant:" << t->Ant << " , Space: " << t->Space
		          << " , Start: " << t->Start.Sub(config.Start)
		          << " , End: " << t->End().Sub(config.Start) << std::endl;
	}
#endif
}

void GeneratedData::GenerateTrajectoriesFor(AntID antID, const AntData &ant) {
	auto prevKey = ant.Keypoints.begin();
	auto nextKey = prevKey + 1;

	AntTrajectory::Ptr current;
	size_t             points = 0;
	for (const auto &[spaceID, t] : Ticks) {
		if (t < prevKey->At) {
			continue;
		}
		// increment keypoints if needed
		while (nextKey != ant.Keypoints.end() &&
		       (t > nextKey->At || nextKey->Space != prevKey->Space)) {
			++nextKey;
			++prevKey;
		}
		if (nextKey == ant.Keypoints.end()) {
			break;
		}

		if (current && current->Space != prevKey->Space) {
			current->Positions.conservativeResize(points, 5);
			Trajectories.push_back(current);
			current.reset();
		}
		if (spaceID != prevKey->Space || prevKey->At > t) {
			continue;
		}

		if (current && t.MonoID() != current->Start.MonoID()) {
			current->Positions.conservativeResize(points, 5);
			Trajectories.push_back(current);
			current.reset();
		}

		// now we are in the right space, ant t is bounded by prevKey and
		// nextKey
		// create trajectory as needed
		if (!current) {
			current        = std::make_shared<AntTrajectory>();
			current->Ant   = antID;
			current->Space = prevKey->Space;
			current->Start = t;
			current->Positions =
			    Eigen::Matrix<double, Eigen::Dynamic, 5>(Ticks.size(), 5);
			points = 0;
		}

		// Add interpolation to current Trajectory
		auto prevPosition =
		    Eigen::Vector3d(prevKey->X, prevKey->Y, prevKey->Angle);
		auto nextPosition =
		    Eigen::Vector3d(nextKey->X, nextKey->Y, nextKey->Angle);
		double ratio = (t.Sub(prevKey->At)).Seconds() /
		               (nextKey->At.Sub(prevKey->At)).Seconds();
		current->Positions(points, 0) = (t.Sub(current->Start)).Seconds();
		current->Positions(points, 4) = 0;
		current->Positions.block<1, 3>(points, 1) =
		    (prevPosition + ratio * (nextPosition - prevPosition)).transpose();
		while (current->Positions(points, 3) < M_PI) {
			current->Positions(points, 3) += 2 * M_PI;
		}
		while (current->Positions(points, 3) >= M_PI) {
			current->Positions(points, 3) -= 2 * M_PI;
		}
		++points;
	}

	if (current) {
		current->Positions.conservativeResize(points, 5);
		Trajectories.push_back(current);
	}
}

void GeneratedData::GenerateInteractions(const Config &config) {
	Interactions.clear();

	for (const auto &[antID, ant] : config.Ants) {
		GenerateInteractionsFor(antID, ant);
	}
	std::sort(
	    Interactions.begin(),
	    Interactions.end(),
	    [](const AntInteraction::Ptr &a, const AntInteraction::Ptr &b) {
		    return a->End < b->End;
	    }
	);
#ifndef NDEBUG
	for (const auto &i : Interactions) {
		std::cerr << "AntInteraction{ IDs:{" << i->IDs.first << ","
		          << i->IDs.second << "}, Start: " << i->Start.Sub(config.Start)
		          << ", End:" << i->End.Sub(config.Start) << "}" << std::endl;
	}
#endif
}

void GeneratedData::GenerateInteractionsFor(AntID antID, const AntData &ant) {
	for (const auto &i : ant.Interactions) {
		auto aSegments = FindTrajectorySegments(i.Other, i.Start, i.End);
		auto bSegments = FindTrajectorySegments(antID, i.Start, i.End);
		if (aSegments.size() != bSegments.size()) {
			throw std::logic_error(
			    "Interactions does not have the same segments size"
			);
		}
		for (size_t ii = 0; ii < aSegments.size(); ++ii) {
			auto res   = std::make_shared<AntInteraction>();
			res->IDs   = {i.Other, antID};
			res->Types = i.Types;
			res->Space = bSegments[ii].Trajectory->Space;
			res->Start =
			    std::min(aSegments[ii].StartTime(), bSegments[ii].StartTime());
			res->End =
			    std::max(aSegments[ii].EndTime(), bSegments[ii].EndTime());
			res->Trajectories = std::make_pair(
			    std::move(aSegments[ii]),
			    std::move(bSegments[ii])
			);
			Interactions.push_back(res);
		}
	}
}

std::vector<AntTrajectorySegment> GeneratedData::FindTrajectorySegments(
    AntID antID, const Time &start, const Time &end
) {
	std::vector<AntTrajectorySegment> results;
	auto                              fi      = Trajectories.begin();
	Time                              current = start;
	while (true) {
		fi = std::find_if(
		    fi,
		    Trajectories.end(),
		    [&](const AntTrajectory::Ptr &t) {
			    bool matches = t->Ant == antID && t->End() > current;
			    return matches;
		    }
		);

		if (fi == Trajectories.end()) {
			throw std::runtime_error("could not find any suitable trajectory");
		}
		AntTrajectorySegment s;
		s.Trajectory = *fi;
		Duration offsetStart, offsetEnd;
		for (s.Begin = 0; s.Begin < s.Trajectory->Positions.rows(); ++s.Begin) {
			offsetStart = s.Trajectory->Positions(s.Begin, 0) *
			              Duration::Second.Nanoseconds();
			if (s.Trajectory->Start.Add(offsetStart) >= current) {
				break;
			}
		}
		for (s.End = s.Begin; s.End < s.Trajectory->Positions.rows(); ++s.End) {
			offsetEnd = s.Trajectory->Positions(s.End, 0) *
			            Duration::Second.Nanoseconds();
			if (s.Trajectory->Start.Add(offsetEnd) > end) {
				break;
			}
		}
		results.push_back(std::move(s));
		if (results.back().End >= results.back().Trajectory->Positions.rows()) {
			current = results.back().EndTime().Add(1);
		} else {
			break;
		}
	}
	return results;
}

void GeneratedData::GenerateFrames(const Config &config) {
	struct TrajectoryIterator {
		size_t             Index;
		AntTrajectory::Ptr Trajectory;

		TrajectoryIterator(const AntTrajectory::Ptr &t)
		    : Index(0)
		    , Trajectory(t) {}

		bool Done() const {
			return !Trajectory || Index >= Trajectory->Positions.rows();
		}

		void Increment() {
			if (Done()) {
				return;
			}
			++Index;
		}

		fort::Time Time() const {
			if (Done()) {
				return Time::Forever();
			}
			return Trajectory->Start.Add(
			    Trajectory->Positions(Index, 0) * Duration::Second.Nanoseconds()
			);
		}
	};

	std::map<AntID, TrajectoryIterator> trajectories;

	Frames.clear();
	Frames.reserve(Ticks.size());
	for (const auto &t : Trajectories) {
		trajectories.insert({t->Ant, TrajectoryIterator(t)});
	}

	for (const auto &[spaceID, time] : Ticks) {
		auto identified       = std::make_shared<IdentifiedFrame>();
		auto collision        = std::make_shared<CollisionFrame>();
		identified->FrameTime = time;
		identified->Space     = spaceID;
		identified->Height    = 1000;
		identified->Width     = 1000;
		identified->Positions = IdentifiedFrame::PositionMatrix(3, 5);

		size_t i = 0;
		for (auto &[antID, current] : trajectories) {
			if (current.Done() == true) {
				auto fi = std::find_if(
				    Trajectories.begin(),
				    Trajectories.end(),
				    [&](const AntTrajectory::Ptr &t) {
					    return t->Ant == current.Trajectory->Ant &&
					           t->Start > current.Trajectory->End();
				    }
				);
				if (fi != Trajectories.end()) {
					current = TrajectoryIterator(*fi);
				}
			}
			if (spaceID != current.Trajectory->Space) {
				continue;
			}
			if (current.Done() || current.Time() > time) {
				continue;
			}
			identified->Positions(i, 0) = antID;
			identified->Positions.block<1, 4>(i, 1) =
			    current.Trajectory->Positions.block<1, 4>(current.Index, 1);
			current.Increment();
			++i;
		}

		identified->Positions.conservativeResize(i, 5);

		collision->FrameTime = time;
		collision->Space     = spaceID;

		for (const auto &i : Interactions) {
			if (i->Space != spaceID || i->Start > time || i->End < time) {
				continue;
			}
			Collision c;
			c.IDs   = i->IDs;
			c.Types = i->Types;
			c.Zone  = 0;
			collision->Collisions.push_back(c);
		}
		Frames.push_back({identified, collision});
	}
}

void GeneratedData::GenerateTagStatistics(const Config &config) {
	for (const auto &[antID, ant] : config.Ants) {
		GenerateTagStatisticsFor(antID - 1, ant);
	}
#ifndef NDEBUG
	for (const auto &[tagID, stats] : Statistics) {
		std::cerr << " + TagID: " << FormatTagID(tagID) << std::endl
		          << " +--+ FirstSeen: " << stats.FirstSeen << std::endl
		          << " +--+ LastSeen: " << stats.LastSeen << std::endl
		          << " +--+ Counts: " << stats.Counts.transpose() << std::endl;
	}
#endif // NDEBUG
}

void GeneratedData::GenerateTagStatisticsFor(
    uint32_t tagID, const AntData &ant
) {
	struct DetectionSegment {
		SpaceID Space;
		Time    Start, End;
	};

	std::vector<DetectionSegment> segments;

	std::shared_ptr<DetectionSegment> current;
	for (const auto &k : ant.Keypoints) {
		if (current && current->Space != k.Space) {
			segments.push_back(*current);
			current.reset();
		}

		if (!current) {
			current        = std::make_shared<DetectionSegment>();
			current->Space = k.Space;
			current->Start = k.At;
			continue;
		}
		current->End = k.At;
	}
	if (current) {
		segments.push_back(*current);
	}

	auto countFrames =
	    [this](const Time &start, const Time &end, SpaceID space) {
		    size_t found(0);
		    Time   lastSeen, firstSeen(Time::SinceEver());
		    for (const auto &[identified, collision] : Frames) {
			    if (identified->FrameTime < start) {
				    continue;
			    }
			    if (identified->FrameTime > end) {
				    break;
			    }
			    if (identified->Space != space) {
				    continue;
			    }
			    lastSeen = identified->FrameTime;
			    if (firstSeen.IsSinceEver()) {
				    firstSeen = identified->FrameTime;
			    }
			    ++found;
		    }
		    return std::make_tuple(found, firstSeen, lastSeen);
	    };

	Statistics[tagID].ID = tagID;
	Statistics[tagID].Counts.resize(10);
	Statistics[tagID].Counts.setZero();

	for (size_t i = 0; i < segments.size(); ++i) {
		const auto &s = segments[i];
		const auto &[found, firstSeen, lastSeen] =
		    countFrames(s.Start, s.End, s.Space);
		Statistics[tagID].Counts(TagStatistics::TOTAL_SEEN) += found;
		Statistics[tagID].LastSeen = lastSeen.Round(1);
		if (i > 0) {
			Statistics[tagID].Counts(priv::TagStatisticsHelper::ComputeGap(
			    segments[i - 1].End,
			    s.Start
			)) += 1;
		} else {
			Statistics[tagID].FirstSeen = firstSeen.Round(1);
		}
	}
}

} // namespace myrmidon
} // namespace fort
