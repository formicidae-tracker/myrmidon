#pragma once

#include <cpptrace/cpptrace.hpp>

namespace fort {

namespace myrmidon {

namespace priv {

// Exception for deleted reference
// @T the type of reference
//
//
// Since <Identification> need to point to their related <Ant>, that
// own the list of the same identification we need to use
// std::weak_ptr in <Identification>. Same problem arises as any
// <Identification> needs a reference to the <Identifier> owning them.
//
// Therefore we should be able to report the case that should never
// happen under normal circonstances (it wouls be considered a real
// bug) where we have an Identification with a <DeletedReference> to
// one of these object.
//
// TODO: maybe it hsould be a std::logic_error
template <typename T> class DeletedReference : public cpptrace::runtime_error {
public:
	// Its Constructor
	//
	// It initialize the reason of the exception to Deleted reference
	// to <T> with <T> actually replaced by the typeid name.
	inline DeletedReference() noexcept
	    : cpptrace::runtime_error(
	          std::string("Deleted reference to ") + typeid(T).name() +
	          " object"
	      ){};

	// Its destructor
	inline virtual ~DeletedReference() noexcept {}
};

} // namespace priv

} // namespace myrmidon

} // namespace fort
