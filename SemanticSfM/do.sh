#!/bin/bash

#./do.sh img_path recon_path use_gms
#i=1
#for i in $@
#do
img_path=$1
seg_path=$2
sfm_path=$1/$3
mvs_path=$sfm_path/mvs

# python build/software/SfM/SfM_SequentialPipeline.py $img_path $sfm_path"_ransac" 0 # normal sfm pipeline
python build/software/SfM/SfM_SequentialPipeline.py $img_path $seg_path $sfm_path"_seg" 0 # with semantic segmentation images
#python build/software/SfM/SfM_SequentialPipeline.py $img_path $sfm_path"_gms_ransac" 1 # with gms

#spend_sfm=$end_sfm-$start_sfm



#done

