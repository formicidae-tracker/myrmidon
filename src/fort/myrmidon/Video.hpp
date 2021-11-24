#pragma once

#include <vector>
#include <functional>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/Typedefs.hpp>
#include <fort/myrmidon/types/Traits.hpp>
#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <fort/myrmidon/types/Collision.hpp>
#include <fort/myrmidon/types/AntTrajectory.hpp>
#include <fort/myrmidon/types/AntInteraction.hpp>
#include <fort/myrmidon/types/MaybeDeref.hpp>

namespace cv {
class Mat;
}

namespace fort {
namespace myrmidon {


struct VideoFrameData {
	uint32_t                         Position;
	fort::Time                       Time;
	IdentifiedFrame::Ptr             Identified;
	CollisionFrame::Ptr              Collided;
	std::vector<AntTrajectory::Ptr>  Trajectories;
	std::vector<AntInteraction::Ptr> Interactions;

	bool Empty() const;

	template <typename T>
	void Append(const T & value);
};


struct VideoSegment {
	typedef std::vector<VideoSegment> List;

	SpaceID                     Space;
	std::string                 AbsoluteFilePath;
	std::vector<VideoFrameData> Data;
	uint32_t                    Begin,End;

	template <typename IterType>
	static void Match(List & list,
	                  IterType begin,
	                  IterType end);

private:
	template <typename IterType>
	static void MatchSortedFiltered(List & list,
	                                IterType begin,
	                                IterType end);

	template <typename IterType>
	IterType Match(IterType begin,
	               IterType end,
	               timed_data);

	template <typename IterType>
	IterType Match(IterType begin,
	               IterType end,
	               time_ranged_data);



};


struct VideoSequence {
	static void ForEach(const VideoSegment::List & list,
	                    std::function<void (cv::Mat & frame,
	                                        const VideoFrameData & data)> operation);
};

template <>
inline void
VideoFrameData::Append(const IdentifiedFrame::Ptr & f) {
	Identified = f;
}
template <>
inline void
VideoFrameData::Append(const CollisionData & data) {
	Identified = std::get<0>(data);
	Collided = std::get<1>(data);
}

template <>
inline void
VideoFrameData::Append(const AntTrajectory::Ptr & t) {
	Trajectories.push_back(t);
}

template <>
inline void
VideoFrameData::Append(const AntInteraction::Ptr & i) {
	Interactions.push_back(i);
}

template <typename IterType>
inline void
VideoSegment::Match(List & list,
                    IterType begin,
                    IterType end) {

	if ( list.empty() ) {
		return;
	}
	if ( std::find_if(list.begin()+1,
	                  list.end(),
	                  [&list](const VideoSegment & s) {
		                  return s.Space != list.front().Space;
	                  }) != list.end() ) {
		throw std::invalid_argument("This implementation only supports matching of segment from the same space");
	}

	SpaceID space = list.front().Space;

	typedef typename std::iterator_traits<IterType>::value_type   Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type> TypeTraits;

	auto compare =
		[](const Type & a, const Type & b) -> bool {
			return TypeTraits::compare(MaybeDeref(a),MaybeDeref(b));
		};

	typedef typename TypeTraits::data_category data_category;
	if constexpr ( TypeTraits::spaced_data == false ) {
			std::sort(begin,end,
			          compare);
			MatchSortedFiltered(list,begin,end);
			return;
	} else {
		std::vector<Type> filtered;
		filtered.reserve(std::distance(begin,end));

		std::copy_if(begin,end,
		             std::back_inserter(filtered),
		             [space](const Type & v) -> bool {
			             return TypeTraits::space(MaybeDeref(v)) == space;
		             });

		std::sort(filtered.begin(),
		          filtered.end(),
		          compare);

		MatchSortedFiltered(list,filtered.begin(),filtered.end());
	}
}

template <typename IterType>
inline void
VideoSegment::MatchSortedFiltered(List & list,
                                  IterType begin,
                                  IterType end) {
	typedef typename std::iterator_traits<IterType>::value_type   Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type> TypeTraits;
	typedef typename TypeTraits::data_category data_category;

	for ( auto & s : list ) {
		begin = s.Match(begin,end,data_category());
		if ( begin == end ) {
			return;
		}
	}
}

template <typename IterType>
inline IterType
VideoSegment::Match(IterType begin,
                    IterType end,
                    timed_data /*placeholder*/) {

	typedef typename std::iterator_traits<IterType>::value_type Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type>    TypeTraits;

	for ( auto & d : Data ) {
		while( TypeTraits::time(MaybeDeref(*begin)) < d.Time ) {
			++begin;
			if ( begin == end ) {
				return end;
			}
		}
		while ( TypeTraits::time(MaybeDeref(*begin)) == d.Time ) {
			d.Append(*begin);
			++begin;
			if ( begin == end ) {
				return end;
			}
		}
	}
	return begin;
}


template <typename IterType>
inline IterType
VideoSegment::Match(IterType begin,
                    IterType end,
                    time_ranged_data /*placeholder*/) {
	typedef typename std::iterator_traits<IterType>::value_type Type;
	typedef data_traits<typename pointed_type_if_any<Type>::type>    TypeTraits;

	//TODO use segment tree to reduce to O(n log(n) ) complexity from O(n2) here
	for ( auto & d : Data ) {
		while( TypeTraits::end(MaybeDeref(*begin)) < d.Time ) {
			++begin;
			if ( begin == end ) {
				return end;
			}
		}
		for ( IterType iter = begin; iter != end; ++iter ) {
			const auto & start = TypeTraits::start(MaybeDeref(*iter));
			const auto & end = TypeTraits::end(MaybeDeref(*iter));
			if ( start <= d.Time && d.Time <= end ) {
				d.Append(*iter);
			}
		}
	}
	return begin;
}


} // namespace myrmidon
} // namespace fort
