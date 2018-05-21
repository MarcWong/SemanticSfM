from PIL import Image
import numpy as np
import tensorflow as tf
from math import ceil
import cv2,colorsys,os
import matplotlib.pyplot as plt
from time import time
from tensorpack.utils.segmentation.segmentation import visualize_label
n_classes = 21



def visualize_prediction(prediction):
    """Visualize prediction."""
    cm = np.argmax(prediction, axis=2) + 1
    color_cm = add_color(cm)
    plt.imshow(color_cm)
    plt.show()

def add_color(img):
    """Color classes a good distance away from each other."""
    h, w = img.shape
    img_color = np.zeros((h, w, 3))
    for i in xrange(1, 151):
        img_color[img == i] = to_color(i)
    return img_color * 255  # is [0.0-1.0]  should be [0-255]

def to_color(category):
    """Map each category color a good distance away from each other on the HSV color space."""
    v = (category-1)*(137.5/360)
    return colorsys.hsv_to_rgb(v, 1, 1)



def is_edge(x,y, data):
    w,h=data.shape
    for d_x in [-1,0,1]:
        for d_y in [-1,0,1]:
            if x + d_x >= w or x+d_x <0:
                continue
            if y + d_y >= h or y+d_y < 0:
                continue
            if data[x + d_x, y + d_y] != data[x,y]:
                return True
    return False


def generate_trimap(rador = 1):
    #main_img_dir = "/data_a/dataset/cityscapes"
    #meta_txt = "cityscapes"

    main_img_dir = "/data_a/dataset/pascalvoc2012/VOC2012trainval/VOCdevkit/VOC2012"
    meta_txt = "pascalvoc12"

    from tensorpack.utils.fs import mkdir_p
    trimap_dir = os.path.join(main_img_dir,"trimap_gt{}".format(rador))
    mkdir_p(trimap_dir)
    print(trimap_dir)
    f = open(os.path.join(meta_txt,"train.txt"))
    result_f = open(os.path.join(meta_txt, "train_tripmap{}.txt".format(rador)),"w")
    lines = f.readlines()
    from tqdm import tqdm
    for l in tqdm(lines):
        l = l.strip("\n")
        img_dir, label_dir = l.split(" ")
        img = cv2.imread(img_dir)
        label = cv2.imread(label_dir,0)
        origin_label = label.copy()
        basename = os.path.basename(label_dir)
        #edge = cv2.Canny(label, 100, 200).astype("float32")
        #xs,ys = np.where(edge==255)
        w,h = label.shape
        for x in range(w):
            for y in range(h):
                if is_edge(x,y,label):
                    origin_label[x-rador:x+rador,y-rador:y+rador] = 255


        tripmap_name = os.path.join(trimap_dir,basename)
        cv2.imwrite(tripmap_name, origin_label)


        result_f.write("{} {}\n".format(img_dir,tripmap_name))
    f.close()
    result_f.close()



def generate_trimap_pascal(rador = 1):
    #main_img_dir = "/data_a/dataset/cityscapes"
    #meta_txt = "cityscapes"

    main_img_dir = "/data_a/dataset/pascalvoc2012/VOC2012trainval/VOCdevkit/VOC2012"
    meta_txt = "pascalvoc12"

    from tensorpack.utils.fs import mkdir_p
    trimap_dir = os.path.join(main_img_dir,"trimap_gt{}".format(rador))
    mkdir_p(trimap_dir)
    print(trimap_dir)
    f = open(os.path.join(meta_txt,"train.txt"))
    result_f = open(os.path.join(meta_txt, "train_tripmap{}.txt".format(rador)),"w")
    lines = f.readlines()
    from tqdm import tqdm
    for l in tqdm(lines):
        l = l.strip("\n")
        img_dir, label_dir = l.split(" ")
        img = cv2.imread(os.path.join(main_img_dir,img_dir))
        label = cv2.imread(os.path.join(main_img_dir,label_dir),0)
        new_label = label.copy()
        basename = os.path.basename(label_dir)
        #edge = cv2.Canny(label, 100, 200).astype("float32")
        #xs,ys = np.where(edge==255)
        w,h = label.shape
        for x in range(w):
            for y in range(h):
                if is_edge(x,y,label):
                    new_label[x-rador:x+rador,y-rador:y+rador] = 255

        tripmap_name = os.path.join(trimap_dir,basename)

        cv2.imshow("im", img / 255.0)
        cv2.imshow("raw-originlabel", label)
        cv2.imshow("color-originlabel", visualize_label(label))
        cv2.imshow("raw-newlabel", new_label)
        cv2.imshow("color-newlabel", visualize_label(new_label))
        cv2.waitKey(0)

        #cv2.imwrite(tripmap_name, new_label)
        result_f.write("{} {}\n".format(img_dir,tripmap_name))
    f.close()
    result_f.close()

generate_trimap_pascal()


