#include "Ant.hpp"

#include "fort/myrmidon/types/Typedefs.hpp"
#include "handle/AntHandle.hpp"

#include "Identification.hpp"
#include "priv/Ant.hpp"

namespace fort {
namespace myrmidon {

Ant::Ant(std::unique_ptr<AntHandle> handle)
    : d_p(std::move(handle)) {}

Ant::~Ant() = default;

TagID Ant::IdentifiedAt(const Time &time) const {
	return d_p->Get().IdentifiedAt(time);
}

const IdentificationList &Ant::Identifications() const {
	return d_p->Identifications();
}

AntID Ant::ID() const {
	return d_p->Get().AntID();
}

const Color &Ant::DisplayColor() const {
	return d_p->Get().DisplayColor();
}

void Ant::SetDisplayColor(const Color &color) {
	d_p->Get().SetDisplayColor(color);
}

Ant::DisplayState Ant::DisplayStatus() const {
	return d_p->Get().DisplayStatus();
}

void Ant::SetDisplayStatus(DisplayState s) {
	d_p->Get().SetDisplayStatus(s);
}

const Value &Ant::GetValue(const std::string &name, const Time &time) const {
	return d_p->Get().GetValue(name, time);
}

void Ant::SetValue(
    const std::string &name, const Value &value, const Time &time
) {
	d_p->Get().SetValue(name, value, time);
}

const std::map<Time, Value> &Ant::GetValues(const std::string &key) const {
	return d_p->Get().GetValues(key);
}

void Ant::DeleteValue(const std::string &name, const Time &time) {
	d_p->Get().DeleteValue(name, time);
}

void Ant::AddCapsule(
    AntShapeTypeID shapeTypeID, const std::shared_ptr<Capsule> &capsule
) {
	d_p->Get().AddCapsule(shapeTypeID, capsule);
}

const TypedCapsuleList &Ant::Capsules() const {
	return d_p->Get().Capsules();
}

void Ant::DeleteCapsule(const size_t index) {
	d_p->Get().DeleteCapsule(index);
}

void Ant::ClearCapsules() {
	d_p->Get().ClearCapsules();
}

} // namespace myrmidon
} // namespace fort

std::ostream &operator<<(std::ostream &out, const fort::myrmidon::Ant &a) {

	out << "Ant{ID:" << fort::myrmidon::FormatAntID(a.ID()) << ",↤";
	std::set<fort::myrmidon::TagID> ids;
	for (const auto &idt : a.Identifications()) {
		ids.insert(idt->TagValue());
	}
	std::string sep = "{";
	for (const auto id : ids) {
		out << sep << fort::myrmidon::FormatTagID(id);
		sep = ",";
	}
	return out << "}}";
}
