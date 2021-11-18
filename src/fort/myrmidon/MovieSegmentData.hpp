#pragma once

#include <any>


namespace fort {
namespace myrmidon {


struct MovieSegmentData {
	struct MatchedData {
		fort::Time                       Time;
		IdentifiedFrame::Ptr             Identified;
		CollisionFrame::Ptr              Collisions;
		std::vector<AntTrajectory::Ptr>  Trajectories;
		std::vector<AntInteraction::Ptr> Interactions;
		std::vector<std::any>            UserData;

		template <typename T>
		void Append(const T & value);
	};

	typedef std::vector<MovieSegmentData> List;

	SpaceID                     Space;
	std::string                 AbsoluteFilePath;
	std::vector<MatchedData>    Data;

	template <typename IterType>
	static void MatchData(List & list,
	                      IterType begin,
	                      IterType end);

private:
	template <typename IterType>
	static void MatchSortedFilteredData(List & list,
	                        IterType begin,
	                        IterType end);

	template <typename IterType>
	IterType MatchData(IterType begin,
	                   IterType end,
	                   timed_data);

	template <typename IterType>
	IterType MatchData(IterType begin,
	                   IterType end,
	                   time_ranged_data);
};

template <typename T>
inline void
MovieSegmentData::MatchedData::Append( const T & value) {
	UserData.push_back(value);
}


template <>
inline void
MovieSegmentData::MatchedData::Append(const IdentifiedFrame::Ptr & f) {
	Identified = f;
}
template <>
inline void
MovieSegmentData::MatchedData::Append(const CollisionData & data) {
	Identified = std::get<0>(data);
}

template <>
inline void
MovieSegmentData::MatchedData::Append(const AntTrajectory::Ptr & t) {
	Trajectories.push_back(t);
}

template <>
inline void
MovieSegmentData::MatchedData::Append(const AntInteraction::Ptr & i) {
	Interactions.push_back(i);
}

template <typename IterType>
inline void
MovieSegmentData::MatchData(List & list,
                            IterType begin,
                            IterType end) {

	if ( list.empty() ) {
		return;
	}
	if ( std::find_if(list.begin()+1,
	                  list.end(),
	                  [&list](const MovieSegmentData & s) {
		                  return s.Space != list.front().Space;
	                  }) != list.end() ) {
		throw std::invalid_argument("This implementation only supports matching of segment from the same space");
	}

	SpaceID space = list.front().Space;

	typedef typename std::iterator_traits<IterType>::value_type Type;
	typedef data_traits<Type> TypeTraits;

	typedef typename TypeTraits::data_category data_category;
	if constexpr ( TypeTraits::spaced_data == false ) {
		std::sort(begin,end,TypeTraits::compare);
		MatchSortedFilteredData(list,begin,end);
		return;
	} else {
		std::vector<Type> filtered;
		filtered.reserve(std::distance(begin,end));

		std::copy_if(begin,end,
		             filtered.begin(),
		             [space](const Type & v) {
			             MaybeDeref(v).Space == space;
		             });

		std::sort(filtered.begin(),
		          filtered.end(),
		          TypeTraits::compare);

		MatchSortedFilteredData(list,filtered.begin(),filtered.end());
	}
}

template <typename IterType>
inline void
MovieSegmentData::MatchSortedFilteredData(List & list,
                                          IterType begin,
                                          IterType end) {

	typedef typename std::iterator_traits<IterType>::value_type Type;
	typedef data_traits<Type> TypeTraits;

	typedef typename TypeTraits::data_category data_category;

	for ( auto & s : list ) {
		begin = s.MatchData(begin,end,data_category());
		if ( begin == end ) {
			return;
		}
	}
}

template <typename IterType>
inline IterType
MovieSegmentData::MatchData(IterType begin,
                            IterType end,
                            timed_data /*placeholder*/) {

	for ( auto & d : Data ) {
		while( MaybeDeref(*begin).Time() < d.Time ) {
			++begin;
			if ( begin == end ) {
				return end;
			}
		}
		while ( MaybeDeref(*begin).Time() == d.Time ) {
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
MovieSegmentData::MatchData(IterType begin,
                            IterType end,
                            time_ranged_data /*placeholder*/) {
	//TODO use segment tree to reduce to O(n log(n) ) complexity from O(n2) here
	for ( auto & d : Data ) {
		while( MaybeCall(MaybeDeref(*begin).End) > d.Time ) {
			++begin;
			if ( begin == end ) {
				return end;
			}
		}
		for ( IterType iter = begin; iter != end; ++iter ) {
			const auto & start = MaybeDeref(*iter).Start;
			const auto & end = MaybeCall(MaybeDeref(*iter).End);
			if ( d.Time <= start && d.Time <= end ) {
				d.Append(*iter);
			}
		}
	}
	return begin;
}


} // namespace myrmidon
} // namespace fort
