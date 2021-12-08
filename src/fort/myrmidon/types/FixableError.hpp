#pragma once

#include <stdexcept>
#include <memory>
#include <vector>
#include <string>

namespace fort {
namespace myrmidon {

class FixableError : public std::runtime_error {
public:
	typedef std::unique_ptr<FixableError> Ptr;

	FixableError(const std::string & reason);
	virtual ~FixableError() noexcept;

	virtual std::string FixDescription() const noexcept = 0;

	virtual void Fix() = 0;
};

typedef std::vector<FixableError::Ptr> FixableErrorList;

class FixableErrors : public FixableError {
public:
	FixableErrors(FixableErrorList errors);
	virtual ~FixableErrors() noexcept;

	const FixableErrorList & Errors() const noexcept;

	std::string FixDescription() const noexcept override;

	void Fix() const;

private:
	static std::string BuildReason(const FixableErrorList & errors) noexcept;

	FixableErrorList d_errors;

};



}  // myrmidon
}  // fort
