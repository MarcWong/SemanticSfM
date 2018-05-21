# Author: Tao Hu <taohu620@gmail.com>


import os
import gzip
import numpy as np
import cv2

from ...utils import logger
from ..base import RNGDataFlow

__all__ = ['CityscapesEdge']


class CityscapesEdge(RNGDataFlow):
    def __init__(self, meta_dir, name,
                 shuffle=None, dir_structure=None):

        assert name in ['train', 'val'], name
        assert os.path.isdir(meta_dir), meta_dir
        self.reset_state()
        self.name = name

        if shuffle is None:
            shuffle = name == 'train'
        self.shuffle = shuffle
        self.imglist = []

        if name == 'train':
            f = open(os.path.join(meta_dir,"train.txt"),"r")
        else:
            f = open(os.path.join(meta_dir, "val.txt"), "r")

        for line in f.readlines():
            self.imglist.append(line.strip("\n").split(" "))
        f.close()

        #self.imglist = self.imglist[:20]

    def size(self):
        return len(self.imglist)

    def get_data(self):
        idxs = np.arange(len(self.imglist))
        if self.shuffle:
            self.rng.shuffle(idxs)
        for k in idxs:
            fname, flabel = self.imglist[k]
            fname = cv2.imread(fname, cv2.IMREAD_COLOR)
            flabel = cv2.imread(flabel, cv2.IMREAD_GRAYSCALE)
            
            edge = cv2.Canny(fname, 100, 200).astype("float32") / 255
            edge = cv2.GaussianBlur(edge, (5, 5), 0)
            yield [fname, flabel, edge]



if __name__ == '__main__':
    pass