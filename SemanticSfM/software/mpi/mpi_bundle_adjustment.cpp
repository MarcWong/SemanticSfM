/// Bundle adjustment to refine Structure; Motion and Intrinsics
#include"mpi_bundle_adjustment.h"

namespace i23dSFM{
    using namespace sfm;
    bool master_bundle_adjustment(SfM_Data & _sfm_data,const bool& _bFixedIntrinsics){
            //拆分数据


            //汇总数据

    }

    bool client_bundle_adjustment(SfM_Data & _sfm_data,const bool& _bFixedIntrinsics){
        Bundle_Adjustment_Ceres::BA_options options;
        if (_sfm_data.GetPoses().size() > 100)
        {
            options._preconditioner_type = ceres::JACOBI;
            options._linear_solver_type = ceres::SPARSE_SCHUR;
        }
        else
        {
            options._linear_solver_type = ceres::DENSE_SCHUR;
        }
        Bundle_Adjustment_Ceres bundle_adjustment_obj(options);
        return bundle_adjustment_obj.Adjust(_sfm_data, true, true, !_bFixedIntrinsics);

    }
}
