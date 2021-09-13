#include "time.h"

#include "Rcpp.h"

#include "RcppEigen.h"

// via the depends attribute we tell Rcpp to create hooks for
// RcppEigen so that the build process will know what to do
//
// [[Rcpp::depends(RcppEigen)]]


// simple example of creating two matrices and
// returning the result of an operatioon on them
//
// via the exports attribute we tell Rcpp to make this function
// available from R
//
// [[Rcpp::export]]
Eigen::MatrixXd rcppeigen_hello_world() {
    Eigen::MatrixXd m1 = Eigen::MatrixXd::Identity(3, 3);
    // Eigen::MatrixXd m2 = Eigen::MatrixXd::Random(3, 3);
    // Do not use Random() here to not promote use of a non-R RNG
    Eigen::MatrixXd m2 = Eigen::MatrixXd::Zero(3, 3);
    for (auto i=0; i<m2.rows(); i++)
        for (auto j=0; j<m2.cols(); j++)
            m2(i,j) = R::rnorm(0, 1);

    return m1 + 3 * (m1 + m2);
}
