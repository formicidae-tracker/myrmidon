#include "Experiment.hpp"

#include "ExperimentReadWriter.hpp"

#include "Ant.hpp"
#include "Identifier.hpp"

#include "../utils/NotYetImplemented.hpp"

namespace fm = fort::myrmidon;
using namespace fm::priv;

Experiment::Experiment(const fs::path & filepath )
	: d_absoluteFilepath(fs::weakly_canonical(filepath))
	, d_basedir(d_absoluteFilepath.parent_path())
	, d_identifier(Identifier::Create())
	, d_threshold(40)
	, d_family(fort::tags::Family::Undefined) {
}

Experiment::Ptr Experiment::Create(const fs::path & filename) {
	return Experiment::Ptr(new Experiment(filename));
}

Experiment::Ptr Experiment::NewFile(const fs::path & filepath) {
	auto absolutePath = fs::weakly_canonical(filepath);
	auto base = absolutePath;
	base.remove_filename();
	auto res = Create(absolutePath);

	fs::create_directories(base);
	res->Save(absolutePath);
	return res;
}


Experiment::Ptr Experiment::Open(const fs::path & filepath) {
	return ExperimentReadWriter::Open(filepath);
}

void Experiment::Save(const fs::path & filepath) const {
	auto basedir = fs::weakly_canonical(d_absoluteFilepath).parent_path();
	auto newBasedir = fs::weakly_canonical(filepath).parent_path();
	//TODO: should not be an error.
	if ( basedir != newBasedir ) {
		throw std::runtime_error("Changing experiment file directory is not yet supported");
	}

	ExperimentReadWriter::Save(*this,filepath);
}

void Experiment::AddTrackingDataDirectory(const TrackingDataDirectory & toAdd) {
	if (d_dataDirs.count(toAdd.URI().generic_string()) != 0 ) {
		throw std::invalid_argument("directory '" + toAdd.URI().string() + "' is already present");
	}

	std::vector<const TrackingDataDirectory*> sortedInTime;
	for(const auto & el : d_dataDirs ) {
		sortedInTime.push_back(&(el.second));
	}
	sortedInTime.push_back(&toAdd);
	auto fi = TimeValid::SortAndCheckOverlap(sortedInTime.begin(),sortedInTime.end());
	if ( fi.first != fi.second ) {
		std::ostringstream os;
		os << *fi.first << " and " << *fi.second << " overlaps in time";
		throw std::invalid_argument(os.str());
	}

	d_dataDirs.insert(std::make_pair(toAdd.URI().generic_string(),toAdd));
}

bool Experiment::ContainsFramePointer()  const  {
	//check if Identification contains any frame
	for(const auto & [ID,a] :  d_identifier->Ants() ) {
		for (const auto & ident : a->Identifications() ) {
			if ( ident->Start() || ident->End() ) {
				return true;
			}
		}
	}

	// TODO check if other things containing FramePointer contains any..

	return false;
}

void Experiment::RemoveTrackingDataDirectory(fs::path path) {

	if ( path.is_absolute() ) {
		fs::path root = d_absoluteFilepath;
		path = fs::relative(path,root.remove_filename());
	}

	if ( d_dataDirs.count(path.generic_string()) == 0 ) {
		throw std::invalid_argument("Could not find data path '" + path.string() + "'");
	}

	if ( ContainsFramePointer() ) {
		throw std::runtime_error("This Experiment contains FramePointer, and therefore removing a TrackingDataDirectory may breaks everything, and is therefore disabled");
	}

	d_dataDirs.erase(path.generic_string());
}

const Experiment::TrackingDataDirectoryByPath & Experiment::TrackingDataDirectories() const {
	return d_dataDirs;
}

const std::string & Experiment::Name() const {
	return d_name;
}

void Experiment::SetName(const std::string & name) {
	d_name = name;
}

const std::string & Experiment::Author() const {
	return d_author;
}

void Experiment::SetAuthor(const std::string & author) {
	d_author = author;
}

const std::string & Experiment::Comment() const {
	return d_comment;
}

void Experiment::SetComment(const std::string & comment) {
	d_comment = comment;
}

uint8_t Experiment::Threshold() const {
	return d_threshold;
}

void Experiment::SetThreshold(uint8_t th) {
	if ( th == 0 ) {
		th = 1;
	}
	if ( th == 255 ) {
		th = 254;
	}
	d_threshold = th;
}

const fs::path & Experiment::AbsoluteFilePath() const {
	return d_absoluteFilepath;
}

const fs::path & Experiment::Basedir() const {
	return d_basedir;
}


fort::tags::Family Experiment::Family() const {
	return d_family;
}

void Experiment::SetFamily(fort::tags::Family tf) {
	d_family = tf;
}
