#pragma once

#include <functional>
#include <vector>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/AntInteraction.hpp>
#include <fort/myrmidon/types/AntTrajectory.hpp>
#include <fort/myrmidon/types/Collision.hpp>
#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <fort/myrmidon/types/MaybeDeref.hpp>
#include <fort/myrmidon/types/Traits.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>

namespace fort {

namespace video {
struct Frame;
}

namespace myrmidon {

/**
 * Represents the tracking data and query results associated with a
 * video frame.
 *
 * @note After a call to Query::FindVideoSegments() all pointer values
 * will be set to `nullptr`. One must call VideoSegment::Match() to
 * query results with the VideoFrameData present in the
 * VideoSegment::List.
 *
 * @warning In the unlikely case of a VideoFrameData without any
 * tracking data ( the video frame was exported but not even a
 * tracking timeout/frame drop was reported), the value of
 * VideoFrameData::Time will be set to Time::SinceEver(), and all
 * other field would be empty or set to `nullptr`.
 */
struct VideoFrameData {
	/**
	 * the video frame position in the video file
	 */
	uint32_t                         Position;
	/**
	 * the video frame acquisition time.
	 */
	fort::Time                       Time;
	/**
	 * The ants position (if previously VideoSegment::Match() 'ed)
	 */
	IdentifiedFrame::Ptr             Identified;
	/**
	 * The ants collision (if previously VideoSegment::Match() 'ed)
	 */
	CollisionFrame::Ptr              Collided;
	/**
	 * The ants trajectories (if previously VideoSegment::Match() 'ed)
	 */
	std::vector<AntTrajectory::Ptr>  Trajectories;
	/**
	 * The ants interactions (if previously VideoSegment::Match() 'ed)
	 */
	std::vector<AntInteraction::Ptr> Interactions;

	/**
	 * Indicates the (unlikely) case where no tracking data is
	 * associated with this video frame.
	 *
	 * @return `true` if there is no tracking data (not even a timeout /
	 * frame drop report) associated with this video frame.
	 */
	bool Empty() const {
		return Time.IsInfinite();
	}

	template <typename T> void Append(const T &value);
};

/**
 * Represents parts of a video file with its associated tracking data.
 *
 * VideoSegment are queried with Query::FindVideoSegments(). Once
 * queried they are blank, i.e. no query results will appears in their
 * Data field. One would call Match() to associate queries results
 * with a VideoSegment::List. Finally one would call
 * VideoSequence::ForEach to iterate over each video frame in the
 * VideoSegment::List.
 *
 * @note Query::FindVideoSegments(), Match() and
 * VideoSequence::ForEach accepts a VideoSegment::List as
 * argument. Indeed it could happen that the desired VideoSequence
 * would span multiple video file.
 *
 * \verbatim embed:rst:leading-asterisk
 * .. code-block:: cpp
 *
 *     #include <fort/myrmidon/Experiment.hpp>
 *     #include <fort/myrmidon/Query.hpp>
 *     #include <fort/myrmidon/Video.hpp>
 *
 *     using namespace fort::myrmidon;
 *
 *     auto e = Experiment::Open("file.myrmidon");
 *
 *     // Note: it would be extremly computationally intensive to iterate
 *     // over the whole experiment, we therefore select a time region.
 *     auto start = fort::Time::Parse("2019-11-02T20:00:00.000Z");
 *     auto end = start.Add(30 * fort::Duration::Second);
 *
 *     // step 0: perform some queries on the experiment.
 *     std::vector<AntTrajectory::Ptr> trajectories;
 *     Query::ComputeAntTrajectoriesArgs args;
 *     args.Start = start;
 *     args.End = end;
 *     Query::ComputeAntTrajectories(e,trajectories,args);
 *
 *     // step 1: look up a VideoSegment::List
 *     VideoSegment::List segments;
 *     Query::FindVideoSegments(e,
 *                              segments,
 *                              1, // space we are looking for
 *                              start,
 *                              end);
 *
 *     // step 2: match the query results with the VideoSegment::List
 *     VideoSegment::Match(segments,trajectories.begin(),trajectories.end());
 *
 *     // step 3: iterate over all video frames
 *     VideoSequence::ForEach(segments,
 *                            [](cv::Mat & frame, const VideoFrameData & data) {
 *                                // step 3.1: on each `frame`, perform an
 * operation based on `data`
 *                            });
 *
 * \endverbatim
 */
struct VideoSegment {
	/**
	 * A std::vector of VideoSegment
	 */
	typedef std::vector<VideoSegment> List;

	/**
	 * The SpaceID of the Space this VideoSegment belongs to.
	 */
	SpaceID                     Space;
	/**
	 * the abolute file path to the video file.
	 */
	std::string                 AbsoluteFilePath;
	/**
	 * Matched queries result and acquisition time for the frames in the
	 * segment.
	 */
	std::vector<VideoFrameData> Data;
	/**
	 * Position of the first video frame in the file for the segment.
	 */
	uint32_t                    Begin;
	/**
	 * Position of the last video frame + 1 in the file for the segment.
	 */
	uint32_t                    End;

	/**
	 * Matches a query result with a VideoSegment::List
	 * @tparam iterator type of the sequence of object to match. These
	 * objects should define fort::myrmidon::data_traits. This is the
	 * case for any result of Query::IdentifyFrames,
	 * Query::CollideFrames, Query::ComputeAntTrajectories and
	 * Query::ComputeAntInteractions
	 * @param list the VideoSegments to associate data with
	 * @param begin start iterator of the sequence to match
	 * @param end past over end iterator of the sequence to match
	 * @throws std::invalid_argument if all VideoSegment in list are
	 * not from the same Space.
	 */
	template <typename IterType>
	static void Match(List &list, IterType begin, IterType end);

private:
	template <typename IterType>
	static void MatchSortedFiltered(List &list, IterType begin, IterType end);

	template <typename IterType>
	IterType Match(IterType begin, IterType end, timed_data);

	template <typename IterType>
	IterType Match(IterType begin, IterType end, time_ranged_data);
};

/**
 * Operations on VideoSegment::List ( as they form a sequence)
 */
struct VideoSequence {
	/**
	 * Iterates over all frames of a sequence.
	 *
	 * @param list the VideoSegment::List to iterate on
	 * @param operation the operation to perform on each video frame
	 * of the sequence.
	 *
	 * \verbatim embed:rst:leading-asterisk
	 * .. code-block:: c++
	 *
	 *     #include <fort/myrmidon/Query.hpp>
	 *     #include <fort/myrmidon/Video.hpp>
	 *
	 *     using namespace fort::myrmidon;
	 *
	 *     VideoSegment::List segments;
	 *     Query::FindVideoSegments(e,
	 *                              segments,
	 *                              1,
	 *                              fort::Time::SinceEver(),
	 *                              fort::Time::Forever());
	 *
	 *     VideoSequence::ForEach(segments,
	 *                            [](const VideoFrameData & data) {
	 *                                // do something on frame based on data
	 *                            });
	 * \endverbatim
	 */
	static void ForEach(
	    const VideoSegment::List &list,
	    std::function<
	        void(const video::Frame &frame, const VideoFrameData &data)>
	        operation
	);
};

template <> inline void VideoFrameData::Append(const IdentifiedFrame::Ptr &f) {
	Identified = f;
}

template <> inline void VideoFrameData::Append(const CollisionData &data) {
	Identified = std::get<0>(data);
	Collided   = std::get<1>(data);
}

template <> inline void VideoFrameData::Append(const AntTrajectory::Ptr &t) {
	Trajectories.push_back(t);
}

template <> inline void VideoFrameData::Append(const AntInteraction::Ptr &i) {
	Interactions.push_back(i);
}

template <typename IterType>
inline void VideoSegment::Match(List &list, IterType begin, IterType end) {

	if (list.empty()) {
		return;
	}
	if (std::find_if(
	        list.begin() + 1,
	        list.end(),
	        [&list](const VideoSegment &s) {
		        return s.Space != list.front().Space;
	        }
	    ) != list.end()) {
		throw std::invalid_argument(
		    "This implementation only supports matching of segment from the "
		    "same space"
		);
	}

	SpaceID space = list.front().Space;

	typedef typename std::iterator_traits<IterType>::value_type   Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type> TypeTraits;

	auto compare = [](const Type &a, const Type &b) -> bool {
		return TypeTraits::compare(MaybeDeref(a), MaybeDeref(b));
	};

	typedef typename TypeTraits::data_category data_category;
	if constexpr (TypeTraits::spaced_data == false) {
		std::sort(begin, end, compare);
		MatchSortedFiltered(list, begin, end);
		return;
	} else {
		std::vector<Type> filtered;
		filtered.reserve(std::distance(begin, end));

		std::copy_if(
		    begin,
		    end,
		    std::back_inserter(filtered),
		    [space](const Type &v) -> bool {
			    return TypeTraits::space(MaybeDeref(v)) == space;
		    }
		);

		std::sort(filtered.begin(), filtered.end(), compare);

		MatchSortedFiltered(list, filtered.begin(), filtered.end());
	}
}

template <typename IterType>
inline void
VideoSegment::MatchSortedFiltered(List &list, IterType begin, IterType end) {
	typedef typename std::iterator_traits<IterType>::value_type   Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type> TypeTraits;
	typedef typename TypeTraits::data_category                    data_category;

	for (auto &s : list) {
		begin = s.Match(begin, end, data_category());
		if (begin == end) {
			return;
		}
	}
}

template <typename IterType>
inline IterType
VideoSegment::Match(IterType begin, IterType end, timed_data /*placeholder*/) {

	typedef typename std::iterator_traits<IterType>::value_type   Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type> TypeTraits;

	for (auto &d : Data) {
		while (TypeTraits::time(MaybeDeref(*begin)) < d.Time) {
			++begin;
			if (begin == end) {
				return end;
			}
		}
		while (TypeTraits::time(MaybeDeref(*begin)) == d.Time) {
			d.Append(*begin);
			++begin;
			if (begin == end) {
				return end;
			}
		}
	}
	return begin;
}

template <typename IterType>
inline IterType VideoSegment::
    Match(IterType begin, IterType end, time_ranged_data /*placeholder*/) {
	typedef typename std::iterator_traits<IterType>::value_type   Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type> TypeTraits;

	// TODO use segment tree to reduce to O(n log(n) ) complexity from O(n2)
	// here
	for (auto &d : Data) {
		while (TypeTraits::end(MaybeDeref(*begin)) < d.Time) {
			++begin;
			if (begin == end) {
				return end;
			}
		}
		for (IterType iter = begin; iter != end; ++iter) {
			const auto &start = TypeTraits::start(MaybeDeref(*iter));
			const auto &end   = TypeTraits::end(MaybeDeref(*iter));
			if (start <= d.Time && d.Time <= end) {
				d.Append(*iter);
			}
		}
	}
	return begin;
}

} // namespace myrmidon
} // namespace fort
