#include "Space.hpp"

#include <fort/myrmidon/priv/Zone.hpp>
#include <fort/myrmidon/priv/Space.hpp>
#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

#include "handle/SpaceHandle.hpp"


namespace fort {
namespace myrmidon {


Space::Space(std::unique_ptr<SpaceHandle> handle)
	: d_p(std::move(handle)) {
}

Space::~Space() = default;

SpaceID Space::ID() const {
	return d_p->Get().ID();
}

const std::string & Space::Name() const {
	return d_p->Get().Name();
}

void Space::SetName(const std::string & name) {
	d_p->Get().SetName(name);
}

Zone::Ptr Space::CreateZone(const std::string & name) {
	return d_p->CreateZone(name);
}

void Space::DeleteZone(ZoneID zoneID) {
	d_p->DeleteZone(zoneID);
}

const ZoneByID & Space::Zones() const {
	return d_p->Zones();
}

std::pair<std::string,uint64_t> Space::LocateMovieFrame(const Time & time) const {
	for ( const auto & tdd : d_p->Get().TrackingDataDirectories() ) {
		if ( tdd->IsValid(time) == false ) {
			continue;
		}

		auto ref = tdd->FrameReferenceAfter(time);
		auto movieSegment = tdd->MovieSegments().Find(time);

		auto movieFrameID = movieSegment.second->ToMovieFrameID(ref.FrameID());
		return std::make_pair(movieSegment.second->AbsoluteFilePath().string(),movieFrameID);
	}
	std::ostringstream oss;
	oss << "Could not find time " << time << " in space " << d_p->Get().Name();
	throw std::out_of_range(oss.str());
}



} // namespace myrmidon
} // namespace fort
