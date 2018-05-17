#ifndef i23dSFM_UTILITY_HPP
#define i23dSFM_UTILITY_HPP

#include "Eigen/Core"

using namespace eigen;

namespace i23dSFM
{
    namespace sfm
    {
        template <typename T>
        Matrix<T, eigen::Dynamic, eigen::Dynamic> Matlab2Eigen(const mwArray mat_array);

        template <typename T>
        mwArray Eigen2Matlab(const Matrix<T, eigen::Dynamic, eigen::Dynamic> eigen_array);
    } // namespace sfm
} // namespace i23dSFM

#endif