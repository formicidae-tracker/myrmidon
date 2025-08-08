#pragma once

#include <cpptrace/exceptions.hpp>
#include <memory>
#include <string>
#include <vector>

namespace fort {
namespace myrmidon {

namespace details {
class WrapLazyException : public cpptrace::lazy_exception {
public:
	explicit WrapLazyException(
	    std::string &&message, cpptrace::lazy_exception &&wrapped
	);
	virtual ~WrapLazyException() noexcept = default;

	const char	             *message() const noexcept override;
	const cpptrace::stacktrace &trace() const noexcept override;

private:
	mutable std::string      d_message;
	cpptrace::lazy_exception d_wrapped;
};
} // namespace details

/**
 * Represents an error that could be potentially fixed.
 *
 * Fixing the error will have most certainly drawbacks, otherwise it
 * would be simpler not to raise anything and clear things up
 * internally.
 */

class FixableError : public details::WrapLazyException {
public:
	/**
	 * A pointer to the error.
	 */
	typedef std::unique_ptr<FixableError> Ptr;

	FixableError(
	    std::string              &&reason,
	    cpptrace::lazy_exception &&origin = cpptrace::lazy_exception{}
	);
	virtual ~FixableError() noexcept = default;

	/**
	 * Description of the fix.
	 *
	 * @return the description of the fix.
	 */
	virtual std::string FixDescription() const noexcept = 0;

	/**
	 * Fix the error.
	 */
	virtual void Fix() = 0;
};

/**
 * A list of FixableError.
 */
typedef std::vector<FixableError::Ptr> FixableErrorList;

/**
 * A collection of FixableError as a FixableError.
 *
 * If you really need to see all the nifty detail, you can use Errors()
 */
class FixableErrors : public FixableError {
public:
	FixableErrors(
	    FixableErrorList           errors,
	    cpptrace::lazy_exception &&origin = cpptrace::lazy_exception{}
	);
	virtual ~FixableErrors() noexcept = default;

	/**
	 * Access indiviudal FixableError
	 * @return the individual FixableError of this FixableErrors
	 */
	const FixableErrorList &Errors() const noexcept;

	std::string FixDescription() const noexcept override;

	void Fix() override;

	FixableErrorList &Errors() noexcept;

private:
	static std::string BuildReason(const FixableErrorList &errors) noexcept;

	FixableErrorList d_errors;
};

} // namespace myrmidon
} // namespace fort
