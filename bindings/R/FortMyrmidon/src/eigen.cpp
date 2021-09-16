#include "../inst/include/FortMyrmidon_types.hpp"

#include "Rcpp.h"

fort::myrmidon::Vector2dList fmVector2dListCreate(SEXP l);

namespace Rcpp {

template <> Eigen::Vector2d as(SEXP vv) {
	NumericVector v(vv);
	if ( v.size() != 2 ) {
		throw std::runtime_error("Converting to Eigen::Vector2d needs a N=numeric vector f size 2");
	}
	return Eigen::Vector2d(v[0],v[1]);
}

template <> SEXP wrap(const Eigen::Vector2d & v) {
	Rcpp::NumericVector res(2);
	res[0] = v.x();
	res[1] = v.y();
	return res;
}

template <> fort::myrmidon::Vector2dList as(SEXP l) {
	if ( l == R_NilValue || Rcpp::is<Rcpp::List>(l)  || Rcpp::is<Rcpp::DataFrame>(l) ) {
		return fmVector2dListCreate(l);
	}
	fort::myrmidon::Vector2dList * obj = Rcpp::internal::as_module_object<fort::myrmidon::Vector2dList>(l);
	return *obj;
}

}

void fmVector2dList_show(const fort::myrmidon::Vector2dList * l) {
	size_t i = 0;
	for(const auto & p : *l) {
		Rcpp::Rcout << "[[" << i << "]]\n"
		            << "[1] " << p.transpose() << "\n";
	}
}

const Eigen::Vector2d & fmVector2dList_at(const fort::myrmidon::Vector2dList * l,size_t index) {
	return l->at(index-1);
}

void fmVector2dList_set(fort::myrmidon::Vector2dList * l,size_t index, const Eigen::Vector2d & v) {
	l->at(index-1) = v;
}

void fmVector2dList_append(fort::myrmidon::Vector2dList * l, SEXP x) {
	try {
		l->push_back(Rcpp::as<Eigen::Vector2d>(x));
		return;
	} catch (const std::exception & e) {
	}
	if ( Rcpp::is<Rcpp::List>(x) ) {
		for ( const auto & v : fmVector2dListCreate(x) ) {
			l->push_back(v);
		}
		return;
	}

	throw std::runtime_error("Could only append NumericVector of size 2 or other fmVectorList");
}

Rcpp::DataFrame fmVector2dList_asDataFrame(const fort::myrmidon::Vector2dList * l) {
	auto s = l->size();
	Rcpp::NumericVector x(s),y(s);
	size_t i = 0;
	for ( const auto & v : *l) {
		x[i] = v.x();
		y[i] = v.y();
		++i;
	}
	return Rcpp::DataFrame::create(Rcpp::Named("x") = x,
	                               Rcpp::Named("y") = y);
}

Rcpp::DataFrame fmVector2dList_asList(const fort::myrmidon::Vector2dList * l) {
	auto s = l->size();
	Rcpp::NumericVector x(s),y(s);
	size_t i = 0;
	for ( const auto & v : *l) {
		x[i] = v.x();
		y[i] = v.y();
		++i;
	}
	return Rcpp::DataFrame::create(Rcpp::Named("x") = x,
	                               Rcpp::Named("y") = y);
}


RCPP_MODULE(eigen) {
	using namespace Rcpp;

	class_<fort::myrmidon::Vector2dList>("fmVector2dList",
	                                     R"rdoc(
A list of 2D point
)rdoc")
		.constructor()
		.const_method("show",
		              &fmVector2dList_show)
		.const_method("size",&fort::myrmidon::Vector2dList::size)
		.const_method("[[",&fmVector2dList_at)
		.method("[[<-",&fmVector2dList_set)
		.const_method("as.data.frame",&fmVector2dList_asDataFrame)
		.const_method("as.list",&fmVector2dList_asList)
		;
}

//' Creates a fmVector2dList from a list of numerical vector or a data.frame
//' @param points a list of numerical vector of size 2 or a data.frame with column 'x' and 'y'
//' @return a fmVector2dList
//' @family fmVector2dList methods
//[[Rcpp::export]]
fort::myrmidon::Vector2dList fmVector2dListCreate(SEXP points = R_NilValue) {
	using namespace Rcpp;
	if ( points == R_NilValue ) {
		return fort::myrmidon::Vector2dList();
	}

	if ( Rcpp::is<Rcpp::DataFrame>(points) ) {
		Rcpp::DataFrame df(points);
		Rcpp::NumericVector x = df["x"];
		Rcpp::NumericVector y = df["y"];
		fort::myrmidon::Vector2dList res;
		res.resize(df.nrows());
		for( size_t i = 0; i < df.nrows(); ++i ) {
			res[i] = Eigen::Vector2d(x[i],y[i]);
		}
		return res;
	}

	Rcpp::List list(points);
	fort::myrmidon::Vector2dList res;
	res.reserve(list.size());
	for ( const auto & v : list ) {
		res.push_back(Rcpp::as<Eigen::Vector2d>(v));
	}
	return res;
}


//[[Rcpp::export]]
void pfmIWantAVector2dList(const fort::myrmidon::Vector2dList & l) {
}
