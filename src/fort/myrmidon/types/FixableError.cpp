#include "FixableError.hpp"

#include <sstream>

namespace fort {
namespace myrmidon {

namespace details {
WrapLazyException::WrapLazyException(
    std::string &&message, cpptrace::lazy_exception &&wrapped
)
    : d_message{std::move(message)}
    , d_wrapped{std::move(wrapped)} {}

const char *WrapLazyException::message() const noexcept {
	return d_message.c_str();
}

const cpptrace::stacktrace &WrapLazyException::trace() const noexcept {
	return d_wrapped.trace();
}

} // namespace details

FixableError::FixableError(
    std::string &&reason, cpptrace::lazy_exception &&origin
)
    : details::WrapLazyException(std::move(reason), std::move(origin)) {}

FixableErrors::FixableErrors(
    FixableErrorList errors, cpptrace::lazy_exception &&origin
)
    : FixableError(BuildReason(errors), std::move(origin))
    , d_errors(std::move(errors)) {}

std::string FixableErrors::BuildReason(const FixableErrorList &errors
) noexcept {
	if (errors.empty() == true) {
		return "no error";
	}
	std::ostringstream oss;
	oss << errors.size() << " error(s):" << std::endl;
	for (const auto &e : errors) {
		oss << "- " << e->what() << std::endl;
	};
	return oss.str();
}

const FixableErrorList &FixableErrors::Errors() const noexcept {
	return d_errors;
}

std::string FixableErrors::FixDescription() const noexcept {
	std::ostringstream oss;
	oss << d_errors.size() << " operation(s):" << std::endl;
	for (const auto &e : d_errors) {
		oss << "- " << e->FixDescription() << std::endl;
	}
	return oss.str();
}

void FixableErrors::Fix() {
	for (const auto &e : d_errors) {
		e->Fix();
	}
}

FixableErrorList &FixableErrors::Errors() noexcept {
	return d_errors;
}

} // namespace myrmidon
} // namespace fort
