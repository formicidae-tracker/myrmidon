#include "../inst/include/FortMyrmidon_types.hpp"


#include "Rcpp.h"


namespace Rcpp {


template <> fort::myrmidon::Color as(SEXP s) {
	Rcpp::IntegerVector v(s);
	if ( v.length() != 3) {
		throw std::invalid_argument("A fmColor is a RGB 8-bit triplet, but " + std::to_string(v.length()) + " are passed");
	}
#define get_channel(Name,Index)	  \
	int Name = v[Index]; \
	if ( Name < 0 || Name > 255) { \
		throw std::invalid_argument("Invalid value " \
		                            + std::to_string(Name) \
		                            + " for " #Name " channel"); \
	}

	get_channel(R,0)
	get_channel(G,1)
	get_channel(B,2)
	return fort::myrmidon::Color(R,G,B);
}



template <> SEXP wrap(const fort::myrmidon::Color & c) {
	return Rcpp::IntegerVector::create(Rcpp::Named("R") = std::get<0>(c),
	                                   Rcpp::Named("G") = std::get<1>(c),
	                                   Rcpp::Named("B") = std::get<2>(c));
}

}


//' Returns myrmidon's default collection of colors
//' @return a list of RGB 8-bit triplets
//' @family fmColor methods
//[[Rcpp::export]]
SEXP fmDefaultPalette() {
	const auto & p = fort::myrmidon::DefaultPalette();
	return Rcpp::wrap(p.begin(),p.end());
}

//' Returns myrmidon' default color for index, wrapping arround
//' @param index the wanted index
//' @return a list of RGB 8-bit triplets
//' @family fmColor methods
//[[Rcpp::export]]
fort::myrmidon::Color fmDefaultPaletteColor(int index) {
	return fort::myrmidon::DefaultPaletteColor(index-1);
}
