#include "Identification.hpp"


#include "handle/IdentificationHandle.hpp"

#include "priv/Identification.hpp"
#include "priv/Ant.hpp"

namespace fort {
namespace myrmidon {

const double Identification::DEFAULT_TAG_SIZE = 0.0;

Identification::Identification(std::unique_ptr<IdentificationHandle> handle)
	: d_p(std::move(handle)) {
}

Identification::~Identification() {}

TagID Identification::TagValue() const {
	return d_p->Get().TagValue();
}

AntID Identification::TargetAntID() const {
	return d_p->Get().Target()->AntID();
}


void Identification::SetStart(const Time & start) {
	d_p->Get().SetStart(start);
}

void Identification::SetEnd(const Time & end) {
	d_p->Get().SetEnd(end);
}

const Time & Identification::Start() const {
	return d_p->Get().Start();
}

const Time & Identification::End() const {
	return d_p->Get().End();
}

Eigen::Vector2d Identification::AntPosition() const {
	return d_p->Get().AntPosition();
}

double Identification::AntAngle() const {
	return d_p->Get().AntAngle();
}

bool Identification::HasUserDefinedAntPose() const {
	return d_p->Get().HasUserDefinedAntPose();
}

void Identification::SetUserDefinedAntPose(const Eigen::Vector2d & antPosition,
                                           double antAngle) {
	d_p->Get().SetUserDefinedAntPose(antPosition,antAngle);
}

void Identification::ClearUserDefinedAntPose() {
	d_p->Get().ClearUserDefinedAntPose();
}


void Identification::SetTagSize(double size) {
	d_p->Get().SetTagSize(size);
}

double Identification::TagSize() const {
	return d_p->Get().TagSize();
}

bool Identification::HasDefaultTagSize() const {
	return d_p->Get().UseDefaultTagSize();
}

OverlappingIdentification::OverlappingIdentification(const priv::Identification & a,
                                                     const priv::Identification & b) noexcept
	: std::runtime_error(Reason(a,b)){
}

OverlappingIdentification::~OverlappingIdentification() {}

std::string OverlappingIdentification::Reason(const priv::Identification & a,
                                              const priv::Identification & b) noexcept {
	std::ostringstream os;
	os << a << " and " << b << " overlaps";
	return os.str();
}

std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Identification & identification) {
	return out << identification.d_p->Get();
}

} // namespace fort
} // namespace myrmidon
