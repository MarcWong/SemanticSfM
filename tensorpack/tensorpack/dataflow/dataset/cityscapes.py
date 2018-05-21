# Author: Tao Hu <taohu620@gmail.com>


import os
import gzip
import numpy as np
import cv2

from ...utils import logger
from ..base import RNGDataFlow

__all__ = ['Cityscapes', 'CityscapesFiles']


class CityscapesFiles(RNGDataFlow):
    def __init__(self, meta_dir, name,
                 shuffle=None, dir_structure=None):

        assert name in ['train', 'val','test'], name
        assert os.path.isdir(meta_dir), meta_dir
        self.reset_state()
        self.name = name

        if shuffle is None:
            shuffle = name == 'train'
        self.shuffle = shuffle
        self.imglist = []

        if name == 'train':
            f = open(os.path.join(meta_dir,"train.txt"),"r")
        elif name=="val":
            f = open(os.path.join(meta_dir, "val.txt"), "r")
        elif name=="test":
            f = open(os.path.join(meta_dir, "test.txt"), "r")
        else:
            raise

        for line in f.readlines():
            self.imglist.append(line.strip("\n").split(" "))
        f.close()

        #self.imglist = self.imglist[:100]

    def size(self):
        return len(self.imglist)

    @staticmethod
    def class_num():
        return 19

    def get_data(self):
        idxs = np.arange(len(self.imglist))
        if self.shuffle:
            self.rng.shuffle(idxs)
        for k in idxs:
            fname, flabel = self.imglist[k]
            yield [fname, flabel]

class Cityscapes(RNGDataFlow):
    def __init__(self, meta_dir, name,
                 shuffle=None, dir_structure=None):

        assert name in ['train', 'val','test'], name
        assert os.path.isdir(meta_dir), meta_dir
        self.reset_state()
        self.name = name

        if shuffle is None:
            shuffle = name == 'train'
        self.shuffle = shuffle
        self.imglist = []

        if name == 'train':
            f = open(os.path.join(meta_dir,"train.txt"),"r")
        elif name=="val":
            f = open(os.path.join(meta_dir, "val.txt"), "r")
        elif name=="test":
            f = open(os.path.join(meta_dir, "test.txt"), "r")
        else:
            raise

        for line in f.readlines():
            self.imglist.append(line.strip("\n").split(" "))
        f.close()

        #self.imglist = self.imglist[:100]

    def size(self):
        return len(self.imglist)

    @staticmethod
    def class_num():
        return 19

    def get_data(self):
        idxs = np.arange(len(self.imglist))
        if self.shuffle:
            self.rng.shuffle(idxs)
        for k in idxs:
            fname, flabel = self.imglist[k]
            fname = cv2.imread(fname, cv2.IMREAD_COLOR)
            flabel = cv2.imread(flabel, cv2.IMREAD_GRAYSCALE)
            yield [fname, flabel]



if __name__ == '__main__':
    from tensorpack.dataflow import *
    class BinaryCityscapes(CityscapesFiles):
        def get_data(self):
            for fname, label in super(BinaryCityscapes, self).get_data():
                with open(fname, 'rb') as f:
                    jpeg = f.read()
                jpeg = np.asarray(bytearray(jpeg), dtype='uint8')
                yield [jpeg, label]


    ds0 = BinaryCityscapes()
    ds1 = PrefetchDataZMQ(ds0, nr_proc=1)
    dftools.dump_dataflow_to_lmdb(ds1, '/data2/dataset/cityscapes.lmdb')