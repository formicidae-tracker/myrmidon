#include "IdentifiedFrame.hpp"

namespace fort {
namespace myrmidon {

bool IdentifiedFrame::Contains(uint64_t antID) const {
	return (Positions.array().col(0) == double(antID)).any();
}

std::tuple<AntID,const Eigen::Ref<const Eigen::Vector3d>,ZoneID> IdentifiedFrame::At(size_t index) const {
	if ( index > Positions.rows() ) {
		throw std::out_of_range(std::to_string(index) + " is out of range [0," + std::to_string(Positions.rows()) + "[");
	}
	AntID antID = AntID(Positions(index,0));
	ZoneID zoneID = ZoneID(Positions(index,4));
	Eigen::Ref<const Eigen::Vector3d> position = Positions.block<1,3>(index,1).transpose();
	return std::make_tuple(antID,position,zoneID);
}

} // namespace myrmidon
} // namespace fort
