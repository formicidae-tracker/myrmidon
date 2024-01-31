#include "Experiment.hpp"

#include "handle/ExperimentHandle.hpp"

#include "priv/AntShapeType.hpp"
#include "priv/Experiment.hpp"
#include "priv/Identifier.hpp"
#include "priv/Measurement.hpp"
#include "priv/TrackingDataDirectory.hpp"
#include "priv/TrackingSolver.hpp"

namespace fort {
namespace myrmidon {

const MeasurementTypeID Experiment::HEAD_TAIL_MEASUREMENT_TYPE_ID =
    priv::Measurement::HEAD_TAIL_TYPE;

Experiment Experiment::OpenUnsafe(const std::string &filepath) {
	priv::Experiment::Ptr e = priv::Experiment::Open(filepath);
	return Experiment(std::make_unique<ExperimentHandle>(e));
}

Experiment Experiment::OpenDataLessUnsafe(const std::string &filepath) {
	// its ok to const cast as we cast back as a const
	priv::Experiment::Ptr e = priv::Experiment::OpenDataLess(filepath);
	return Experiment(std::make_unique<ExperimentHandle>(e));
}

Experiment Experiment::CreateUnsafe(const std::string &filepath) {
	priv::Experiment::Ptr e = priv::Experiment::Create(filepath);
	return Experiment(std::make_unique<ExperimentHandle>(e));
}

void Experiment::Save(const std::string &filepath) {
	d_p->Get().Save(filepath);
}

std::string Experiment::AbsoluteFilePath() const {
	return d_p->Get().AbsoluteFilePath().string();
}

Space::Ptr Experiment::CreateSpace(const std::string &name) {
	return d_p->CreateSpace(name);
}

void Experiment::DeleteSpace(SpaceID spaceID) {
	d_p->DeleteSpace(spaceID);
}

const SpaceByID &Experiment::Spaces() const {
	return d_p->Spaces();
}

std::string Experiment::AddTrackingDataDirectory(
    SpaceID spaceID, const std::string &filepath, bool fixCorruptedData
) {
	auto fi = d_p->Get().Spaces().find(spaceID);
	if (fi == d_p->Get().Spaces().end()) {
		throw std::out_of_range("Unknown SpaceID " + std::to_string(spaceID));
	}
	priv::TrackingDataDirectory::Ptr tdd;
	FixableErrorList                 errors;
	try {
		std::tie(tdd, errors) =
		    priv::TrackingDataDirectory::Open(filepath, d_p->Get().Basedir());
	} catch (const std::exception &e) {
		throw std::runtime_error(e.what());
	}
	if (errors.empty() == false) {
		if (fixCorruptedData == false) {
			throw FixableErrors(std::move(errors));
		} else {
			FixableErrors(std::move(errors)).Fix();
			tdd->SaveToCache();
		}
	}
	d_p->Get().AddTrackingDataDirectory(fi->second, tdd);
	return tdd->URI();
}

void Experiment::RemoveTrackingDataDirectory(const std::string &URI) {
	d_p->Get().DeleteTrackingDataDirectory(URI);
}

Ant::Ptr Experiment::CreateAnt() {
	return d_p->CreateAnt();
}

const AntByID &Experiment::Ants() const {
	return d_p->Ants();
}

void Experiment::DeleteAnt(AntID antID) {
	try {
		d_p->DeleteAnt(antID);
	} catch (const priv::AlmostContiguousIDContainer<AntID, priv::Ant>::
	             UnmanagedObject &) {
		throw std::out_of_range("Unknown AntID " + std::to_string(antID));
	}
}

Identification::Ptr Experiment::AddIdentification(
    AntID antID, TagID tagID, const Time &start, const Time &end
) {
	return d_p->AddIdentification(antID, tagID, start, end);
}

void Experiment::DeleteIdentification(const Identification::Ptr &identification
) {
	try {
		d_p->DeleteIdentification(identification);
	} catch (const priv::Identifier::UnmanagedIdentification &e) {
		throw std::invalid_argument(e.what());
	}
}

std::tuple<fort::Time, fort::Time>
Experiment::FreeIdentificationRangeAt(TagID tagID, const Time &time) const {
	fort::Time start, end;
	if (d_p->Get().Identifier()->FreeRangeContaining(start, end, tagID, time) ==
	    false) {
		std::ostringstream oss;
		oss << fort::myrmidon::FormatTagID(tagID) << " identifies an Ant at "
		    << time;
		throw std::runtime_error(oss.str());
	}
	return {start, end};
}

const std::string &Experiment::Name() const {
	return d_p->Get().Name();
}

void Experiment::SetName(const std::string &name) {
	d_p->Get().SetName(name);
}

const std::string &Experiment::Author() const {
	return d_p->Get().Author();
}

void Experiment::SetAuthor(const std::string &author) {
	d_p->Get().SetAuthor(author);
}

const std::string &Experiment::Comment() const {
	return d_p->Get().Comment();
}

void Experiment::SetComment(const std::string &comment) {
	d_p->Get().SetComment(comment);
}

fort::tags::Family Experiment::Family() const {
	return d_p->Get().Family();
}

double Experiment::DefaultTagSize() const {
	return d_p->Get().DefaultTagSize();
}

void Experiment::SetDefaultTagSize(double defaultTagSize) {
	d_p->Get().SetDefaultTagSize(defaultTagSize);
}

MeasurementTypeID Experiment::CreateMeasurementType(const std::string &name) {
	return d_p->Get().CreateMeasurementType(name)->MTID();
}

void Experiment::DeleteMeasurementType(MeasurementTypeID mTypeID) {
	d_p->Get().DeleteMeasurementType(mTypeID);
}

void Experiment::SetMeasurementTypeName(
    MeasurementTypeID mTypeID, const std::string &name
) {
	auto fi = d_p->Get().MeasurementTypes().find(mTypeID);
	if (fi == d_p->Get().MeasurementTypes().end()) {
		throw std::out_of_range(
		    "Unknown MeasurementTypeID " + std::to_string(mTypeID)
		);
	}
	fi->second->SetName(name);
}

std::map<MeasurementTypeID, std::string>
Experiment::MeasurementTypeNames() const {
	std::map<MeasurementTypeID, std::string> res;
	for (const auto &[mtID, mt] : d_p->Get().MeasurementTypes()) {
		res.insert(std::make_pair(mtID, mt->Name()));
	}
	return res;
}

AntShapeTypeID Experiment::CreateAntShapeType(const std::string &name) {
	return d_p->Get().CreateAntShapeType(name)->TypeID();
}

std::map<AntShapeTypeID, std::string> Experiment::AntShapeTypeNames() const {
	std::map<AntShapeTypeID, std::string> res;
	for (const auto &[shapeTypeID, shapeType] : d_p->Get().AntShapeTypes()) {
		res.insert(std::make_pair(shapeTypeID, shapeType->Name()));
	}
	return res;
}

void Experiment::SetAntShapeTypeName(
    AntShapeTypeID antShapeTypeID, const std::string &name
) {
	auto fi = d_p->Get().AntShapeTypes().find(antShapeTypeID);
	if (fi == d_p->Get().AntShapeTypes().end()) {
		throw std::out_of_range(
		    "Unknown AntShapeTypeID " + std::to_string(antShapeTypeID)
		);
	}
	fi->second->SetName(name);
}

void Experiment::DeleteAntShapeType(AntShapeTypeID antShapeTypeID) {
	d_p->Get().DeleteAntShapeType(antShapeTypeID);
}

void Experiment::SetMetaDataKey(const std::string &name, Value defaultValue) {
	d_p->Get().SetMetaDataKey(name, defaultValue);
}

void Experiment::DeleteMetaDataKey(const std::string &key) {
	d_p->Get().DeleteMetaDataKey(key);
}

std::map<std::string, Value> Experiment::MetaDataKeys() const {
	std::map<std::string, Value> res;
	for (const auto &[name, key] : d_p->Get().AntMetadataPtr()->Keys()) {
		res.insert(std::make_pair(name, key->DefaultValue()));
	}
	return res;
}

void Experiment::RenameMetaDataKey(
    const std::string &oldName, const std::string &newName
) {
	d_p->Get().RenameMetaDataKey(oldName, newName);
}

Experiment::Experiment(std::unique_ptr<ExperimentHandle> handle)
    : d_p(std::move(handle)) {}

Experiment::~Experiment() = default;

std::map<AntID, TagID> Experiment::IdentificationsAt(
    const Time &time, bool removeUnidentifiedAnt
) const {
	return d_p->Get().Identifier()->IdentificationsAt(
	    time,
	    removeUnidentifiedAnt
	);
}

TrackingSolver::Ptr Experiment::CompileTrackingSolver(bool collisionsIgnoreZones
) const {
	auto privateSolver = std::make_shared<priv::TrackingSolver>(
	    d_p->Get().Identifier(),
	    d_p->Get().CompileCollisionSolver(collisionsIgnoreZones)
	);
	return std::unique_ptr<TrackingSolver>(new TrackingSolver(privateSolver));
}

void Experiment::EnsureAllDataIsLoaded(
    ProgressReporter::Ptr &&progress, bool fixCorruptedData
) const {
	d_p->Get().EnsureAllDataIsLoaded(std::move(progress), fixCorruptedData);
}

} // namespace myrmidon
} // namespace fort
