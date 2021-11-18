#pragma once

#include <map>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include "Typedefs.hpp"

namespace fort {
namespace myrmidon {
/**
 * Statistics about a TagID in the experiment.
 */
struct TagStatistics {
	/** A map of TagStatistics indexed by TagID */
	typedef std::map<TagID,TagStatistics>   ByTagID;
	/** A vector of count. */
	typedef Eigen::Matrix<uint64_t,Eigen::Dynamic,1> CountVector;
	// Designating each index of <CountVector>
	enum CountHeader {
	    // Number of time the <TagID> was seen in the <Experiment>
		TOTAL_SEEN        = 0,
		// Number of time the <TagID> was seen multiple time in the same Frame.
		MULTIPLE_SEEN     = 1,
		// Number of time their was a gap less than 500 milliseconds were the tracking was lost.
		GAP_500MS         = 2,
		// Number of time their was a gap less than 1 second were the tracking was lost.
		GAP_1S            = 3,
		// Number of time their was a gap less than 10 seconds were the tracking was lost.
		GAP_10S           = 4,
		// Number of times their was a gap less than 1 minute were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_1M            = 5,
		// Number of times their was a gap less than 10 minutes were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_10M           = 6,
		// Number of times their was a gap less than 1 hour were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_1H            = 7,
		// Number of times their was a gap less than 10 hours were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_10H           = 8,
		// Number of times their was a gap of more than 10 hours were the tracking was lost. If using multiple space in an experiment, consider only smaller gap, and add all columns from <GAP_1M> up to this one to consider only gap bigger than 10S.
		GAP_MORE          = 9,
	};

	// The <TagID> this statistics refers too
	TagID       ID;
	// The first <Time> in the <Experiment> this <TagID> was detected.
	Time        FirstSeen;
	// The last <Time> in the <Experiment> this <TagID> was detected.
	Time        LastSeen;
	// Counts were the tag was seen
	CountVector Counts;
};

} // namespace myrmidon
} // namespace fort
