# Author: Tao Hu <taohu620@gmail.com>


import os
import gzip
import numpy as np
import cv2

from tensorpack.utils import logger
from tensorpack.dataflow.base import RNGDataFlow

__all__ = ['Camvid', 'CamvidFiles']


class Camvid(RNGDataFlow):
    def __init__(self, dir, meta_dir, name,
                 shuffle=None):

        assert name in ['train', 'val', 'train_val', 'test'], name
        assert os.path.isdir(dir), dir
        self.reset_state()
        self.dir = dir
        self.name = name

        if shuffle is None:
            shuffle = (name == 'train')
        self.shuffle = shuffle
        self.imglist = []

        if name == 'train':
            f = open(os.path.join(meta_dir,"train.txt"),"r")
        elif name =="val":
            f = open(os.path.join(meta_dir, "val.txt"), "r")
        elif name =="train_val":
            f = open(os.path.join(meta_dir, "train_val.txt"), "r")
        elif name == "test":
            f = open(os.path.join(meta_dir, "test.txt"), "r")
        else:
            raise

        for line in f.readlines():
            self.imglist.append(line.strip("\n").split(" "))
        f.close()

        #self.imglist = self.imglist[:20]

    def size(self):
        return len(self.imglist)

    @staticmethod
    def class_num():
        return 11 #
        #Sky, Building, Pole, Road, Pavement, Tree, SignSymbol, Fence, Car, Pedestrian, Bicyclist, Unlabelled
        #0,1,2,3,4,5,6,7,8,9,10,11

    def get_data(self):
        idxs = np.arange(len(self.imglist))
        if self.shuffle:
            self.rng.shuffle(idxs)
        for k in idxs:
                fname, flabel = self.imglist[k]
                fname = os.path.join(self.dir, fname)
                flabel = os.path.join(self.dir,flabel)
                fname = cv2.imread(fname, cv2.IMREAD_COLOR)
                flabel = cv2.imread(flabel, cv2.IMREAD_GRAYSCALE)
                yield [fname, flabel]


class CamvidFiles(RNGDataFlow):
    def __init__(self, dir, meta_dir, name,
                 shuffle=None):

        assert name in ['train', 'val', 'train_val', 'test'], name
        assert os.path.isdir(dir), dir
        self.reset_state()
        self.dir = dir
        self.name = name

        if shuffle is None:
            shuffle = (name == 'train')
        self.shuffle = shuffle
        self.imglist = []

        if name == 'train':
            f = open(os.path.join(meta_dir,"train.txt"),"r")
        elif name =="val":
            f = open(os.path.join(meta_dir, "val.txt"), "r")
        elif name =="train_val":
            f = open(os.path.join(meta_dir, "train_val.txt"), "r")
        elif name == "test":
            f = open(os.path.join(meta_dir, "test.txt"), "r")
        else:
            raise

        for line in f.readlines():
            self.imglist.append(line.strip("\n").split(" "))
        f.close()

        #self.imglist = self.imglist[:20]

    def size(self):
        return len(self.imglist)

    @staticmethod
    def class_num():
        return 11 #
        #Sky, Building, Pole, Road, Pavement, Tree, SignSymbol, Fence, Car, Pedestrian, Bicyclist, Unlabelled
        #0,1,2,3,4,5,6,7,8,9,10,11

    def get_data(self):
        idxs = np.arange(len(self.imglist))
        if self.shuffle:
            self.rng.shuffle(idxs)
        for k in idxs:
                fname, flabel = self.imglist[k]
                fname = os.path.join(self.dir, fname)
                flabel = os.path.join(self.dir,flabel)
                #fname = cv2.imread(fname, cv2.IMREAD_COLOR)
                #flabel = cv2.imread(flabel, cv2.IMREAD_GRAYSCALE)
                yield [fname, flabel]




if __name__ == '__main__':
    pass