#pragma once

#include "git.h"

namespace fort {
namespace myrmidon {
static inline const std::string_view Version() {
	return git_Describe();
}

static inline const std::string_view Sha1() {
	return git_CommitSHA1();
}
} // namespace myrmidon
} // namespace fort
