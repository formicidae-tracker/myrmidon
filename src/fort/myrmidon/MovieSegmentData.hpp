#pragma once

#include <any>
#include <fort/myrmidon/Types.hpp>


namespace fort {
namespace myrmidon {

template <typename T>
const fort::Time & GetStart(const T & value);

template <typename T>
fort::Duration GetDuration(const T & value);

template <typename T>
SpaceID GetSpace(const T & value);

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

	SpaceID                     ID;
	std::string                 AbsoluteFilePath;
	std::vector<MatchedData>    Data;

	template <typename IterType>  IterType MatchData(IterType begin,
	                                                 IterType end,
	                                                 bool sortFirst = true);

	template <typename IterType>  static void MatchData(List & list,
	                                                    IterType begin,
	                                                    IterType end);

};


template <typename T>
struct static_assert_false : public std::false_type {};

template <typename T>
int TimeCompare(const T & value,
                const fort::Time & time) {
	static_assert(static_assert_false<T>::value,"The matched type must specialize int fort::myrmidon::TimeLocate(const T & value,const fort::Time & time)");
	return -1;
}

template <typename T>
bool SpaceCompare(const T & value,
                  SpaceID spaceID) {
	static_assert(static_assert_false<T>::value,"The matched type must specialize bool fort::myrmidon::SpaceCompare(const T & value,SpaceID spaceID)");
	return false;
}



template <>
inline int
TimeCompare(const IdentifiedFrame::Ptr & value,
            const fort::Time & time) {
	if ( time.Before(value->FrameTime) ) {
		return -1;
	}
	if ( time.Equals(value->FrameTime) ) {
		return 0;
	}
	return 1;
}

template <>
inline int
TimeCompare(const CollisionData & value,
            const fort::Time & time) {
	return TimeCompare<IdentifiedFrame::Ptr>(value.first,time);
}

template <>
inline int
TimeCompare(const AntTrajectory::Ptr & value,
            const fort::Time & time) {
	if ( time.Before(value->Start) ) {
		return -1;
	}
	if ( time.After(value->End()) ) {
		return 1;
	}
	return 0;
}

template <>
inline int
TimeCompare(const AntInteraction::Ptr & value,
            const fort::Time & time) {
	if ( time.Before(value->Start) ) {
		return -1;
	}
	if ( time.After(value->End) ) {
		return 1;
	}
	return 0;
}


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
inline IterType MovieFrameSegment::MatchData(IterType begin,
                                             IterType end) {
	for ( const auto & d : Data ) {
		while(SpaceCompare(*begin,Space) == false
		      || TimeCompare(*begin,d.Time) > 0 ) {
			++begin;
			if ( begin == end ) {
				return end;
			}
		}
		for( IterType iter = begin;
		     iter != end;
		     ++iter) {
			if ( SpaceCompare(*iter,Space) == false ) {
				continue;
			}
			if ( TimeCompare(*iter,d.Time) == 0 ) {
				// Space and Time are correct
				d.Append(*iter);
			}
		}
	}
	return begin;
}

template <typename IterType>
inline static void
MovieFrameSegment::MatchData(List & list,
                             IterType begin,
                             IterType end) {
	std::map<fort::Time,IterType> byEnd,byStart;
	for ( auto iter = begin; iter != end; ++iter) {
		byEnd =
	}

	for ( auto & s : list) {
		begin = s.MatchData(begin,end);
		if ( begin == end ) {
			return;
		}
	}
}

} // namespace myrmidon
} // namespace fort
