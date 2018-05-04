//
// Created by haibao637 on 17-11-4.
//

#ifndef I23DSFM_MPI_BUNDLE_ADJUSTMENT_H
#define I23DSFM_MPI_BUNDLE_ADJUSTMENT_H
#include"i23dSFM/sfm/sfm_data.hpp"
#include "i23dSFM/sfm/sfm_data_BA_ceres.hpp"
#include "ceres/ceres.h"
namespace i23dSFM{
   using namespace sfm;
    bool master_bundle_adjustment(SfM_Data & _sfm_data,const bool& _bFixedIntrinsics);

    bool client_bundle_adjustment(SfM_Data & _sfm_data,const bool& _bFixedIntrinsics);
}



#endif //I23DSFM_MPI_BUNDLE_ADJUSTMENT_H

