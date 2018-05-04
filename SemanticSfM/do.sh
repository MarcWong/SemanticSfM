#!/bin/bash

#./do.sh img_path recon_path use_gms
#i=1
#for i in $@
#do
img_path=$1
sfm_path=$1/$2
mvs_path=$sfm_path/mvs

python build/software/SfM/SfM_SequentialPipeline.py $img_path $sfm_path"_ransac" 0
#python build/software/SfM/SfM_SequentialPipeline.py $img_path $sfm_path"_gms_ransac" 1

#spend_sfm=$end_sfm-$start_sfm



#done

