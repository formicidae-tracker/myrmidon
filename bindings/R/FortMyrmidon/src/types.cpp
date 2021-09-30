#include "types.h"

#include "time.h"

using namespace fort::myrmidon;

#include <Rcpp.h>

#define Vectorize(...) {__VA_ARGS__}
#define CreateDataFrameOptimized(VarName,elements,names,nrows ) \
	Rcpp::List VarName(elements); \
	VarName.attr("names") = Rcpp::CharacterVector(names); \
	VarName.attr("row.names") = Rcpp::IntegerVector::create(NA_INTEGER,(nrows)); \
	VarName.attr("class") = "data.frame"

#define VectorFromEigen(Type,Var,Mat,Col,Size) Rcpp::Type Var(&((Mat)(0,Col)),&((Mat)(0,Col))+Size)

Rcpp::DataFrame fmIdentifiedFrame_Positions(fort::myrmidon::IdentifiedFrame & frame) {

	Eigen::Matrix<double,Eigen::Dynamic,5> ColMajored = frame.Positions;
	VectorFromEigen(NumericVector,IDs,ColMajored,0,ColMajored.rows());
	VectorFromEigen(NumericVector,X,ColMajored,1,ColMajored.rows());
	VectorFromEigen(NumericVector,Y,ColMajored,2,ColMajored.rows());
	VectorFromEigen(NumericVector,Angle,ColMajored,3,ColMajored.rows());
	VectorFromEigen(NumericVector,Zone,ColMajored,4,ColMajored.rows());

	CreateDataFrameOptimized(df,
	                         Vectorize(IDs,X,Y,Angle,Zone),
	                         Vectorize("antID","x","y","angle","zone"),
	                         IDs.size());
	return df;
}

std::string fmInteractionTypes_asStr(const fort::myrmidon::InteractionTypes & types ) {
	std::ostringstream res;
	for ( size_t i = 0; i < types.rows(); ++i ) {
		res << types(i,0) <<  "-" <<  types(i,1) << ",";
	}
	auto ress = res.str();
	ress.pop_back();
	return ress;
}

Rcpp::DataFrame fmAntTrajectory_Positions(const fort::myrmidon::AntTrajectory & t) {
	size_t nPoints = t.Positions.rows();
	VectorFromEigen(NumericVector,time,t.Positions,0,nPoints);
	VectorFromEigen(NumericVector,x,t.Positions,1,nPoints);
 	VectorFromEigen(NumericVector,y,t.Positions,2,nPoints);
 	VectorFromEigen(NumericVector,angle,t.Positions,3,nPoints);
	VectorFromEigen(NumericVector,zone,t.Positions,4,nPoints);
	CreateDataFrameOptimized(df,
	                         Vectorize(time,x,y,angle,zone),
	                         Vectorize("time","x","y","angle","zone"),
	                         t.Positions.rows());
	return df;
}
