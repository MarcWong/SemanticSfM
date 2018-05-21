# Author: Tao Hu <taohu620@gmail.com>
import numpy as np
from pycocotools.coco import COCO
from pycocotools import mask

def generate_id2trainid(_coco):
    new_dict = {}
    for idx,data in enumerate(_coco.cats.items()):
        key,value = data #start from 1!!!
        new_dict[key] = idx+1

    return new_dict


def generate_image_mask(_coco, img_mask, annId, cat_dict):
    height,width,_ =img_mask.shape
    ann = _coco.loadAnns(annId)[0]

    # polygon
    if type(ann['segmentation']) == list:
        for _instance in ann['segmentation']:
            rle = mask.frPyObjects([_instance], height, width)
            m = mask.decode(rle)
            img_mask[np.where(m == 1)] = cat_dict[ann['category_id']]
    # mask
    else:  # mostly is aeroplane
        if type(ann['segmentation']['counts']) == list:
            rle = mask.frPyObjects([ann['segmentation']], height, width)
        else:
            rle = [ann['segmentation']]
        m = mask.decode(rle)
        img_mask[np.where(m == 1)] = cat_dict[ann['category_id']]



    return img_mask, ann

