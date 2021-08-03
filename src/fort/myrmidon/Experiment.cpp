#include "Experiment.hpp"

#include "priv/Experiment.hpp"
#include "priv/TrackingSolver.hpp"
#include "priv/AntShapeType.hpp"
#include "priv/Identifier.hpp"
#include "priv/Measurement.hpp"
#include "priv/TrackingDataDirectory.hpp"
#include "handle/ExperimentHandle.hpp"


namespace fort {
namespace myrmidon {

const MeasurementTypeID Experiment::HEAD_TAIL_MEASUREMENT_TYPE_ID = priv::Measurement::HEAD_TAIL_TYPE;


Experiment::Ptr Experiment::Open(const std::string & filepath) {
	priv::Experiment::Ptr e = priv::Experiment::Open(filepath);
	return Experiment::Ptr(new Experiment(std::make_unique<ExperimentHandle>(e)));
}

Experiment::Ptr Experiment::OpenDataLess(const std::string & filepath) {
	// its ok to const cast as we cast back as a const
	priv::Experiment::Ptr e = priv::Experiment::OpenDataLess(filepath);
	return Experiment::Ptr(new Experiment(std::make_unique<ExperimentHandle>(e)));
}

Experiment::Ptr Experiment::Create(const std::string & filepath) {
	priv::Experiment::Ptr e = priv::Experiment::Create(filepath);
	return Experiment::Ptr(new Experiment(std::make_unique<ExperimentHandle>(e)));
}

void Experiment::Save(const std::string & filepath) {
	d_p->Get().Save(filepath);
}

std::string Experiment::AbsoluteFilePath() const {
	return d_p->Get().AbsoluteFilePath().string();
}

Space::Ptr Experiment::CreateSpace(const std::string & name) {
	return d_p->CreateSpace(name);
}


void Experiment::DeleteSpace(SpaceID spaceID) {
	d_p->DeleteSpace(spaceID);
}

const SpaceByID & Experiment::Spaces() const {
	return d_p->Spaces();
}

std::string Experiment::AddTrackingDataDirectory(SpaceID spaceID,
                                                 const std::string & filepath) {
	auto fi  = d_p->Get().Spaces().find(spaceID);
	if ( fi == d_p->Get().Spaces().end() ) {
		throw std::out_of_range("Unknown Space::ID " + std::to_string(spaceID));
	}
	auto tdd = priv::TrackingDataDirectory::Open(filepath,d_p->Get().Basedir());
	d_p->Get().AddTrackingDataDirectory(fi->second,tdd);
	return tdd->URI();
}

void Experiment::RemoveTrackingDataDirectory(const std::string & URI) {
	d_p->Get().DeleteTrackingDataDirectory(URI);
}


Ant::Ptr Experiment::CreateAnt() {
	return d_p->CreateAnt();
}

const AntByID & Experiment::Ants() const {
	return d_p->Ants();
}

void Experiment::DeleteAnt(AntID antID) {
	d_p->DeleteAnt(antID);
}


Identification::Ptr Experiment::AddIdentification(AntID antID,
                                                  TagID tagID,
                                                  const Time & start,
                                                  const Time & end) {
	return d_p->AddIdentification(antID,tagID,start,end);
}

void Experiment::DeleteIdentification(const Identification::Ptr & identification) {
	d_p->DeleteIdentification(identification);
}


std::tuple<fort::Time,fort::Time>
Experiment::FreeIdentificationRangeAt(TagID tagID, const Time & time) const {
	fort::Time start,end;
	if ( d_p->Get().Identifier()->FreeRangeContaining(start,end,tagID,time) == false ) {
		std::ostringstream oss;
		oss << fort::myrmidon::FormatTagID(tagID) << " identifies an Ant at " << time;
		throw std::runtime_error(oss.str());
	}
	return {start,end};
}



const std::string & Experiment::Name() const {
	return d_p->Get().Name();
}

void Experiment::SetName(const std::string & name) {
	d_p->Get().SetName(name);
}

const std::string & Experiment::Author() const {
	return d_p->Get().Author();
}

void Experiment::SetAuthor(const std::string & author) {
	d_p->Get().SetAuthor(author);
}

const std::string & Experiment::Comment() const {
	return d_p->Get().Comment();
}

void Experiment::SetComment(const std::string & comment) {
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

MeasurementTypeID Experiment::CreateMeasurementType(const std::string & name) {
	return d_p->Get().CreateMeasurementType(name)->MTID();
}

void Experiment::DeleteMeasurementType(MeasurementTypeID mTypeID) {
	d_p->Get().DeleteMeasurementType(mTypeID);
}

void Experiment::SetMeasurementTypeName(MeasurementTypeID mTypeID,
                                        const std::string & name) {
	auto fi = d_p->Get().MeasurementTypes().find(mTypeID);
	if ( fi == d_p->Get().MeasurementTypes().end() ) {
		throw std::invalid_argument("Unknwon measurement type " + std::to_string(mTypeID));
	}
	fi->second->SetName(name);
}


std::map<MeasurementTypeID,std::string> Experiment::MeasurementTypeNames() const {
	std::map<MeasurementTypeID,std::string> res;
	for ( const auto & [mtID,mt] : d_p->Get().MeasurementTypes() ) {
		res.insert(std::make_pair(mtID,mt->Name()));
	}
	return res;
}

AntShapeTypeID Experiment::CreateAntShapeType(const std::string & name) {
	return d_p->Get().CreateAntShapeType(name)->TypeID();
}

std::map<AntShapeTypeID,std::string> Experiment::AntShapeTypeNames() const {
	std::map<AntShapeTypeID,std::string> res;
	for ( const auto & [shapeTypeID,shapeType] : d_p->Get().AntShapeTypes() ) {
		res.insert(std::make_pair(shapeTypeID,shapeType->Name()));
	}
	return res;
}

void Experiment::SetAntShapeTypeName(AntShapeTypeID antShapeTypeID,
                                     const std::string & name) {
	auto fi = d_p->Get().AntShapeTypes().find(antShapeTypeID);
	if ( fi == d_p->Get().AntShapeTypes().end() ) {
		throw std::invalid_argument("unknow AntShapeTypeID " + std::to_string(antShapeTypeID));
	}
	fi->second->SetName(name);
}

void Experiment::DeleteAntShapeType(AntShapeTypeID antShapeTypeID) {
	d_p->Get().DeleteAntShapeType(antShapeTypeID);
}


void Experiment::SetMetaDataKey(const std::string & name,
                                AntStaticValue defaultValue) {
	d_p->Get().SetMetaDataKey(name,defaultValue);
}

void Experiment::DeleteMetaDataKey(const std::string & key) {
	d_p->Get().DeleteMetaDataKey(key);
}


std::map<std::string,std::pair<AntMetaDataType,AntStaticValue>>
Experiment::MetaDataKeys() const {
	std::map<std::string,std::pair<AntMetaDataType,AntStaticValue>> res;
	for ( const auto & [name,key] : d_p->Get().AntMetadataPtr()->Keys() ) {
		res.insert(std::make_pair(name,std::make_pair(key->Type(),key->DefaultValue())));
	}
	return res;
}


void Experiment::RenameMetaDataKey(const std::string & oldName,
                                   const std::string & newName) {
	d_p->Get().RenameMetaDataKey(oldName,newName);

}

Experiment::Experiment(std::unique_ptr<ExperimentHandle> handle)
	: d_p(std::move(handle)) {
}

Experiment::~Experiment() = default;

TrackingDataDirectoryInfo buildTddInfos(const priv::TrackingDataDirectory::Ptr & tdd) {
	return {
	        .URI = tdd->URI(),
	        .AbsoluteFilePath = tdd->AbsoluteFilePath().string(),
	        .Frames = tdd->EndFrame() - tdd->StartFrame() + 1,
	        .Start  = tdd->Start(),
	        .End = tdd->End(),
	};
}


SpaceDataInfo 	buildSpaceInfos( const priv::Space::ConstPtr & space ) {
	SpaceDataInfo res = {.URI = space->URI(),.Name = space->Name(), .Frames = 0 , .Start = Time(), .End = Time()};
	const auto & tdds = space->TrackingDataDirectories();
	if ( tdds.empty() == true ) {
		return res;
	}
	res.Start = tdds.front()->Start();
	res.End = tdds.back()->End();
	res.TrackingDataDirectories.reserve(tdds.size());
	for ( const auto & tdd : tdds ) {
		res.TrackingDataDirectories.push_back(buildTddInfos(tdd));
		res.Frames += res.TrackingDataDirectories.back().Frames;
	}
	return res;
}


ExperimentDataInfo Experiment::GetDataInformations() const {
	ExperimentDataInfo res = { .Frames = 0, .Start = Time::Forever(), .End = Time::SinceEver() };
	const auto & spaces = d_p->Get().Spaces();
	for ( const auto & [spaceID,space] : spaces ) {
		auto sInfo = buildSpaceInfos(space);
		res.Start = std::min(res.Start,sInfo.Start);
		res.End = std::max(res.End,sInfo.End);
		res.Frames += sInfo.Frames;
		res.Spaces[spaceID] = sInfo;
	}

	return res;
}

std::map<AntID,TagID> Experiment::IdentificationsAt(const Time & time,
                                                    bool removeUnidentifiedAnt) const {
	return d_p->Get().Identifier()->IdentificationsAt(time,removeUnidentifiedAnt);
}

TrackingSolver Experiment::CompileTrackingSolver() const {
	return TrackingSolver(std::make_shared<priv::TrackingSolver>(d_p->Get().Identifier(),
	                                                             d_p->Get().CompileCollisionSolver()));
}



} // namespace mrymidon
} // namespace fort
