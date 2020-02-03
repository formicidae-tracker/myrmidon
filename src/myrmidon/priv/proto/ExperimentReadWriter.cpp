#include "ExperimentReadWriter.hpp"

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Ant.hpp>

#include <myrmidon/ExperimentFile.pb.h>

#include "FileReadWriter.hpp"
#include "IOUtils.hpp"

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

ExperimentReadWriter::ExperimentReadWriter() {}
ExperimentReadWriter::~ExperimentReadWriter() {}

Experiment::Ptr ExperimentReadWriter::DoOpen(const fs::path & filename) {
	typedef FileReadWriter<pb::FileHeader,pb::FileLine> ReadWriter;
	auto res = Experiment::Create(filename);
	ReadWriter::Read(filename,
	                 [filename](const pb::FileHeader & h) {
		                 if (h.majorversion() != 0 || h.minorversion() != 1 ) {
			                 std::ostringstream os;
			                 os << "unexpected version " << h.majorversion() << "." << h.minorversion()
			                    << " in " << filename
			                    << " can only works with 0.1";
			                 throw std::runtime_error(os.str());
		                 }
	                 },
	                 [&res,filename](const pb::FileLine & line) {
		                 if (line.has_experiment() == true ) {
			                 IOUtils::LoadExperiment(*res, line.experiment());
		                 }

		                 if (line.has_antdata() == true ) {
			                 IOUtils::LoadAnt(*res, line.antdata());
		                 }
	                 });
	return res;
}


void ExperimentReadWriter::DoSave(const Experiment & experiment, const fs::path & filepath) {
	typedef FileReadWriter<pb::FileHeader,pb::FileLine> ReadWriter;
	pb::FileHeader h;
	h.set_majorversion(0);
	h.set_minorversion(1);

	std::vector<std::function < void ( pb::FileLine &) > > lines;

	lines.push_back([&experiment](pb::FileLine & line) {
		                IOUtils::SaveExperiment(line.mutable_experiment(),experiment);
	                });

	std::vector<fort::myrmidon::Ant::ID> antIDs;
	for (const auto & [ID,a] : experiment.ConstIdentifier().Ants() ) {
		antIDs.push_back(ID);
	}
	std::sort(antIDs.begin(),antIDs.end(),[](fort::myrmidon::Ant::ID a,
	                                         fort::myrmidon::Ant::ID b) -> bool {
		                                      return a < b;
	                                      });

	for (const auto & ID : antIDs) {
		lines.push_back([&experiment,ID](pb::FileLine & line) {
			                IOUtils::SaveAnt(line.mutable_antdata(),
			                                 experiment.ConstIdentifier().Ants().find(ID)->second);
		                });
	}

	ReadWriter::Write(filepath,h,lines);
}


} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort