#include "CollisionSolver.hpp"

#include "AntShapeType.hpp"
#include "KDTree.hpp"
#include "Space.hpp"
#include "fort/myrmidon/priv/Zone.hpp"
#include "fort/myrmidon/types/IdentifiedFrame.hpp"
#include "fort/time/Time.hpp"
#include <fort/myrmidon/Shapes.hpp>
#include <stdexcept>

namespace fort {
namespace myrmidon {
namespace priv {

CollisionSolver::CollisionSolver(
    const SpaceByID &spaces, const AntByID &ants, bool ignoreZones
)
    : d_ignoreZones(ignoreZones) {

	// Deep copy ant shape data.
	for (const auto &[aID, ant] : ants) {
		d_antGeometries.insert(std::make_pair(aID, ant->Capsules()));
	}

	// compiles Zoner per time.

	typedef TimeMap<ZoneID, ZoneGeometry::ConstPtr> ZoneGeometriesByTime;
	for (const auto &[spaceID, space] : spaces) {
		std::set<Time> times = {fort::Time::SinceEver()};
		d_zoneIDs.insert(std::make_pair(spaceID, std::vector<ZoneID>()));
		// we compile all definitions change over time for a space.
		ZoneGeometriesByTime geometries;
		for (const auto &[zID, zone] : space->Zones()) {

			d_zoneIDs.at(spaceID).push_back(zID);
			for (const auto &definition : zone->Definitions()) {
				geometries.InsertOrAssign(
				    zID,
				    std::make_shared<ZoneGeometry>(definition->Shapes()),
				    definition->Start()
				);
				times.insert(definition->Start());
				if (definition->End().IsForever() == false) {
					geometries.InsertOrAssign(
					    zID,
					    std::make_shared<ZoneGeometry>(Shape::List()),
					    definition->End()
					);
					times.insert(definition->End());
				}
			}
		}

		// In this space, we collect all zones ant put it in the right zoner.
		for (const auto &time : times) {
			std::vector<std::pair<ZoneID, Zone::Geometry::ConstPtr>>
			    currentGeometries;
			for (const auto &zID : d_zoneIDs.at(spaceID)) {
				try {
					currentGeometries.push_back({zID, geometries.At(zID, time)}
					);
				} catch (const std::exception &e) {
					continue;
				}
			}

			d_spaceZoners.InsertOrAssign(
			    spaceID,
			    std::make_shared<AntZoner>(currentGeometries),
			    time
			);
		}
	}
}

void CollisionSolver::ComputeCollisions(
    CollisionFrame &collision, IdentifiedFrame &frame
) const {
	if (d_spaceZoners.Count(frame.Space) == 0) {
		throw cpptrace::invalid_argument(
		    "Unknown SpaceID " + std::to_string(frame.Space) +
		    " in IdentifiedFrame"
		);
	}

	LocatedAnts locatedAnts;
	LocateAnts(locatedAnts, frame);
	collision.FrameTime = frame.FrameTime;
	collision.Space     = frame.Space;
	collision.Collisions.clear();
	for (const auto &[zID, ants] : locatedAnts) {
		ComputeCollisions(collision.Collisions, ants, zID);
	}
}

AntZoner::ConstPtr CollisionSolver::ZonerFor(const IdentifiedFrame &frame
) const {
	try {
		return d_spaceZoners.At(frame.Space, frame.FrameTime);
	} catch (const cpptrace::out_of_range &e) {
		throw cpptrace::invalid_argument(
		    "Unknown SpaceID " + std::to_string(frame.Space) +
		    " in collision solver: " + std::string(e.what()) +
		    d_spaceZoners.DebugString()
		);
	}
}

AntZoner::AntZoner(const ZoneGeometries &zoneGeometries)
    : d_zoneGeometries(zoneGeometries) {}

void AntZoner::LocateAnts(
    IdentifiedFrame::PositionMatrix &ants, ZonePriority priority
) const {
	if (ants.cols() <= 4) {
		return;
	}
	size_t zoneDepth = ants.cols() - 4;
	switch (priority) {
	case ZonePriority::PREDECENCE_LOWER:
		locateAntsLower(ants);
		break;
	case ZonePriority::PREDECENCE_HIGHER:
		locateAntsHigher(ants);
		break;
	}
}

void AntZoner::locateAntsLower(IdentifiedFrame::PositionMatrix &ants) const {
	size_t zoneDepth = ants.cols() - 4;
	for (size_t i = 0; i < ants.rows(); ++i) {
		size_t matched{0};
		for (auto iter = d_zoneGeometries.begin();
		     iter != d_zoneGeometries.end();
		     ++iter) {
			if (iter->second->Contains(ants.block<1, 2>(i, 1).transpose()) ==
			    false) {
				continue;
			}
			ants(i, 4 + matched) = iter->first;
			if (++matched >= zoneDepth) {
				break;
			}
		}
	}
}

void AntZoner::locateAntsHigher(IdentifiedFrame::PositionMatrix &ants) const {
	size_t zoneDepth = ants.cols() - 4;
	for (size_t i = 0; i < ants.rows(); ++i) {
		size_t matched{0};
		for (auto iter = d_zoneGeometries.rbegin();
		     iter != d_zoneGeometries.rend();
		     ++iter) {
			if (iter->second->Contains(ants.block<1, 2>(i, 1).transpose()) ==
			    false) {
				continue;
			}
			ants(i, 4 + matched) = iter->first;
			if (++matched >= zoneDepth) {
				break;
			}
		}
	}
}

void CollisionSolver::LocateAnts(
    LocatedAnts &locatedAnts, IdentifiedFrame &frame
) const {

	size_t zoneDepth = frame.Positions.cols() - 4;
	// now for each geometry. we test if the ants is in the zone
	for (size_t i = 0; i < frame.Positions.rows(); ++i) {
		size_t islandIndex = 0;
		if (d_ignoreZones == false && zoneDepth > 0) {
			islandIndex = frame.Positions(i, 4);
		}
		locatedAnts[islandIndex].push_back(frame.Positions.row(i));
	}
}

void CollisionSolver::ComputeCollisions(
    std::vector<Collision>                   &result,
    const std::vector<PositionedAntConstRef> &ants,
    ZoneID                                    zoneID
) const {

	// first-pass we compute possible interactions
	struct AntTypedCapsule {
		Capsule          C;
		AntID            ID;
		AntShapeType::ID TypeID;

		inline bool operator<(const AntTypedCapsule &other) {
			return ID < other.ID;
		}

		inline bool operator>(const AntTypedCapsule &other) {
			return ID > other.ID;
		}

		inline bool operator!=(const AntTypedCapsule &other) {
			return ID != other.ID;
		}
	};

	typedef KDTree<AntTypedCapsule, double, 2> KDT;

	std::vector<KDT::Element> nodes;

	for (const auto &ant : ants) {
		AntID antID  = ant(0, 0);
		auto  fiGeom = d_antGeometries.find(antID);
		if (fiGeom == d_antGeometries.end()) {
			continue;
		}
		Isometry2Dd antToOrig(ant(0, 3), ant.block<1, 2>(0, 1).transpose());

		for (const auto &[typeID, c] : fiGeom->second) {
			auto data = AntTypedCapsule{
			    .C      = c->Transform(antToOrig),
			    .ID     = antID,
			    .TypeID = typeID,
			};
			nodes.push_back({.Object = data, .Volume = data.C.ComputeAABB()});
		}
	}
	auto kdt = KDT::Build(nodes.begin(), nodes.end(), -1);
	std::list<std::pair<AntTypedCapsule, AntTypedCapsule>> possibleCollisions;
	auto                                                   inserter =
	    std::inserter(possibleCollisions, possibleCollisions.begin());
	kdt->ComputeCollisions(inserter);

	// now do the actual collisions
	std::map<InteractionID, std::set<std::pair<uint32_t, uint32_t>>> res;
	for (const auto &coarse : possibleCollisions) {
		if (coarse.first.C.Intersects(coarse.second.C) == true) {
			InteractionID ID =
			    std::make_pair(coarse.first.ID, coarse.second.ID);
			auto type =
			    std::make_pair(coarse.first.TypeID, coarse.second.TypeID);
			res[ID].insert(type);
		}
	}
	result.reserve(result.size() + res.size());
	for (const auto &[ID, interactionSet] : res) {
		InteractionTypes interactions(interactionSet.size(), 2);
		size_t           i = 0;
		for (const auto &t : interactionSet) {
			interactions(i, 0) = t.first;
			interactions(i, 1) = t.second;
			++i;
		}
		result.push_back(Collision{ID, interactions, zoneID});
	}
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
