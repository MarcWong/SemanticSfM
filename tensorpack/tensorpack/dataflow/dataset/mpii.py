# Author: Tao Hu <taohu620@gmail.com>


import os
import gzip
import numpy as np
import cv2
import random
from tensorpack.utils import logger
from tensorpack.dataflow.base import RNGDataFlow
import json
from tensorpack.utils.skeleton import visualization
import scipy
import copy
__all__ = ['mpii']




label_type = "Gaussian"
sigma = 1
nr_skeleton = 16

class mpii(RNGDataFlow):
    def __init__(self, img_dir, meta_dir, name, data_shape, output_shape,
                 shuffle=None):

        assert name in ['train', 'val'], name

        self.reset_state()
        self.data_shape = data_shape
        self.output_shape = output_shape
        self.nr_skeleton = nr_skeleton
        self.meta_dir = meta_dir
        self.name = name
        self.img_dir = img_dir

        with open(meta_dir) as anno_file:
            ann_list = json.load(anno_file)

        if shuffle is None:
            shuffle = name == 'train'
        self.shuffle = shuffle
        self.imglist = []

        for ann in ann_list:
            if name == "train":
                if ann['isValidation'] == 0:# and "055069054" in ann['img_paths']:
                    self.imglist.append(ann)
            elif name == "val":
                if ann['isValidation'] == 1:
                    self.imglist.append(ann)
            else:
                raise

        #self.imglist = self.imglist[:200]


    def size(self):
        return len(self.imglist)

    @staticmethod
    def joint_num():
        return 16

    def get_data(self):
        idxs = np.arange(len(self.imglist))
        if self.shuffle:
            self.rng.shuffle(idxs)
        for k in idxs:
            current_skeleton_obj = copy.deepcopy(self.imglist[k]) #copy is important!!!
            current_skeleton_obj["img_paths"] = os.path.join(self.img_dir, current_skeleton_obj["img_paths"])
            yield current_skeleton_obj





if __name__ == '__main__':
    mm = mpii('/data1/dataset/mpii/images','/home/hutao/lab/tensorpack-forpr/examples/Hourglass/metadata/mpii_annotations.json',
              "train",(256,256),(64,64),shuffle=True)
    ll = mm.imglist
    i = 0
    for data in mm.get_data():
        img = data[0]
        feat = data[1]
        i +=1

