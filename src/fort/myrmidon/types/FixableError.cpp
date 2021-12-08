#include "FixableError.hpp"

#include <sstream>

namespace fort {
namespace myrmidon {

FixableError::FixableError(const std::string & reason)
	: std::runtime_error(reason) {
}

FixableError::~FixableError() {}

FixableErrors::FixableErrors(FixableErrorList errors)
	: FixableError(BuildReason(errors))
	, d_errors(std::move(errors)) {
}

FixableErrors::~FixableErrors() {}

std::string FixableErrors::BuildReason(const FixableErrorList & errors) noexcept {
	if ( errors.empty() == true ) {
		return "no error";
	}
	std::ostringstream oss;
	oss << errors.size() << " error(s):" << std::endl;
	for ( const auto & e : errors ) {
		oss << "- " << e->what() << std::endl;
	};
	return oss.str();
}

const FixableErrorList & FixableErrors::Errors() const noexcept {
	return d_errors;
}

std::string FixableErrors::FixDescription() const noexcept {
	std::ostringstream oss;
	oss << d_errors.size() << " operation(s):" << std::endl;
	for ( const auto & e : d_errors ) {
		oss << "- " << e->FixDescription() << std::endl;
	}
	return oss.str();
}

void FixableErrors::Fix() const {
	for ( const auto & e : d_errors ) {
		e->Fix();
	}
}

}  // myrmidon
}  // fort
