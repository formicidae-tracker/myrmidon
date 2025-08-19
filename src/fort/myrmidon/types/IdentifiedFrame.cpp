#include "IdentifiedFrame.hpp"

#include <cpptrace/cpptrace.hpp>

namespace fort {
namespace myrmidon {

bool IdentifiedFrame::Contains(uint64_t antID) const {
	return (Positions.array().col(0) == double(antID)).any();
}

std::tuple<
    AntID,
    const Eigen::Ref<const Eigen::Vector3d>,
    const Eigen::Ref<const Eigen::VectorXd>>
IdentifiedFrame::At(size_t index) const {
	if (index > Positions.rows()) {
		throw cpptrace::out_of_range(
		    std::to_string(index) + " is out of range [0," +
		    std::to_string(Positions.rows()) + "["
		);
	}

	AntID                             antID = AntID(Positions(index, 0));
	Eigen::Ref<const Eigen::Vector3d> position =
	    Positions.block<1, 3>(index, 1).transpose();
	Eigen::Ref<const Eigen::VectorXd> zones =
	    Positions.block(index, 4, 1, Positions.cols() - 4).transpose();
	return std::make_tuple(antID, position, zones);
}

} // namespace myrmidon
} // namespace fort
