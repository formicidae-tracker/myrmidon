#pragma once

#include <stdexcept>
#include <memory>
#include <vector>
#include <string>

namespace fort {
namespace myrmidon {

/**
 * Represents an error that could be potentially fixed.
 *
 * Fixing the error will have most certainly drawbacks, otherwise it
 * would be simpler not to raise anything and clear things up
 * internally.
 */

class FixableError : public std::runtime_error {
public:
	/**
	 * A pointer to the error.
	 */
	typedef std::unique_ptr<FixableError> Ptr;

	FixableError(const std::string & reason);
	virtual ~FixableError() noexcept;

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
	FixableErrors(FixableErrorList errors);
	virtual ~FixableErrors() noexcept;

	/**
	 * Access indiviudal FixableError
	 * @return the individual FixableError of this FixableErrors
	 */
	const FixableErrorList & Errors() const noexcept;

	std::string FixDescription() const noexcept override;

	void Fix() override;

	FixableErrorList & Errors() noexcept;


private:
	static std::string BuildReason(const FixableErrorList & errors) noexcept;

	FixableErrorList d_errors;

};



}  // myrmidon
}  // fort
