#include "Fakedata.hpp"

#include "GeneratedData.hpp"

namespace fort {
namespace myrmidon {

Fakedata::Fakedata(const fs::path & basedir) {
	try {
		BuildFakeData(basedir);
	} catch (const std::exception & e) {
		CleanUpFilesystem();
		throw;
	}
}

Fakedata::~Fakedata() {
	CleanUpFilesystem();
}



void Fakedata::CleanUpFilesystem() {
	if (d_basedir.empty()) {
		return;
	}
	fs::remove_all(d_basedir);
};

void Fakedata::BuildFakeData(const fs::path & basedir) {
	auto start = Time::Now();
	GenerateFakedata();
	d_basedir = basedir;
	WriteFakedata();
	std::cerr << "Generated data in " << Time::Now().Sub(start) << std::endl;
}


void Fakedata::GenerateFakedata() {
	GeneratedData gen(d_config);
	SaveFullExpectedResult(gen);
	GenerateTruncatedResults();
}

void Fakedata::SaveFullExpectedResult(const GeneratedData & gen) {
	ExpectedResult full;
	full.Start = Time::SinceEver();
	full.End = Time::Forever();
	full.MaximumGap = Duration::Hour;
	full.Trajectories = gen.Trajectories;
	full.Interactions = gen.Interactions;
	d_results.push_back(full);
}

void Fakedata::GenerateTruncatedResults() {
}


void Fakedata::WriteFakedata() {
}

} // namespace myrmidon
} // namespace fort
