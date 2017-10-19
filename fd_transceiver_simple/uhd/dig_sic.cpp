#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include "dig_sic.hpp"


/* Returns the Toeplitz matrix of signal
 * @param l: considered signal length
 * @param k: est. channel length is 2*k
 * @param dim: non-linear dimension is dim
 * @return A: the constructed Toeplitz matrix is with size l * (2*k*dim)
 */
Eigen::MatrixXf sig_toeplitz(Eigen::VectorXf &sig, int l, int k, int dim)
{
    Eigen::MatrixXf A(l, (2*k+1)*dim);
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < 2*k+1; j++) {
            for (int p = 0; p < dim; p++) {
                A(i, j*dim + p) = pow(sig(j+i), p+1);
            }
        }
    }
    return A;
}

/* Returns the estimate self-interfence (SI) channel
 * @param
 */
Eigen::VectorXf si_chnl_est(Eigen::VectorXf &tx_sig, Eigen::VectorXf &rx_sig, int l, int k)
{
    if (tx_sig.size() != rx_sig.size()) {
        std::cout << "Unmatched TX/RX preamble lengths" << std::endl;
        exit(0);
    }
    // matrix w/ dimenstion l * 2k
    Eigen::MatrixXf A = sig_toeplitz(tx_sig, l, k, 1);
	Eigen::BDCSVD <Eigen::MatrixXf> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::VectorXf h = svd.solve(rx_sig.segment(k,l));

    return h;
}

// apply digital sic
Eigen::VectorXf dig_sic(
    Eigen::VectorXf &tx_data,
    Eigen::VectorXf &rx_data,
    Eigen::VectorXf &SI_chnl,
    int l,
    int k
){
    if (tx_data.size() != tx_data.size()) {
        std::cout << "Unmatched TX/RX data lengths" << std::endl;
        exit(0);
    }

    Eigen::MatrixXf A = sig_toeplitz(tx_data, l, k, 1);
    return rx_data.segment(k,l) - A*SI_chnl;
}
