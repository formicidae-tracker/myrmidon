#pragma once


#include "Rcpp.h"


template <typename T> SEXP wrapMap(const std::map<uint32_t,T> & m) {
	if (m.empty()) {
		return Rcpp::List::create();
	}
	uint32_t maxKey = m.rbegin()->first;
	if ( maxKey > 10* m.size() ) {
		throw std::logic_error("Map is too sparse");
	}
	Rcpp::List res(maxKey,R_NilValue);
	for ( const auto & [ID,object] : m ) {
		res[ID-1] = Rcpp::wrap<T>(object);
	}
	return res;
}
