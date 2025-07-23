#pragma once

#include <fort/myrmidon/priv/ExperimentReadWriter.hpp>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

// Saves Experiment using protocol buffer
//
// This <ExperimentReadWriter> read and saves data using protocol
// buffer.
class ExperimentReadWriter : public priv::ExperimentReadWriter {
public:
	// Constructor
	ExperimentReadWriter();
	// Destructor
	virtual ~ExperimentReadWriter();

	// Implements DoOpen
	ExperimentPtr DoOpen(
	    const fs::path &filename, const std::optional<OpenArguments> &openData
	) override;

	// Implements DoSave
	void
	DoSave(const Experiment &experiment, const fs::path &filename) override;
};

} // namespace proto
} // namespace priv
} // namespace myrmidon
} // namespace fort
