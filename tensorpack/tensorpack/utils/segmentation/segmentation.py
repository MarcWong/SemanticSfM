#!/usr/bin/env python
# -*- coding: utf-8 -*-
# File: segmentation.py
# Author: Tao Hu <taohu620@gmail.com>

import numpy as np
from math import ceil
import cv2,colorsys
import pydensecrf.densecrf as dcrf
import os, sys
from tensorpack.utils import logger
from tensorpack.utils.palette import PALETTE_RGB

__all__ = ['update_confusion_matrix', 'predict_slider']

# Colour map. #BGR order.
label_colours = [(0,0,0)
                # 0=background
                ,(128,0,0),(0,128,0),(128,128,0),(0,0,128),(128,0,128)
                # 1=aeroplane, 2=bicycle, 3=bird, 4=boat, 5=bottle
                ,(0,128,128),(128,128,128),(64,0,0),(192,0,0),(64,128,0)
                # 6=bus, 7=car, 8=cat, 9=chair, 10=cow
                ,(192,128,0),(64,0,128),(192,0,128),(64,128,128),(192,128,128)
                # 11=diningtable, 12=dog, 13=horse, 14=motorbike, 15=person
                ,(0,64,0),(128,64,0),(0,192,0),(128,192,0),(0,64,128)]
                # 16=potted plant, 17=sheep, 18=sofa, 19=train, 20=tv/monitor

id_to_name = {
    0:"background",
    1:"aeroplane",
    2:"bicycle",
    3:"bird",
    4:"boat",
    5:"bottle",
    6:"bus",
    7:"car",
    8:"cat",
    9:"chair",
    10:"cow",
    11:"diningtable",
    12:"dog",
    13:"horse",
    14:"motorbike",
    15:"person",
    16:"plant",
    17:"sheep",
    18:"sofa",
    19:"train",
    20:"tv/monitor"
}


ignore_color = (255,255,255)
fuzzy_color = (64,0,128)

label2_colours = [(192,128,0),(64,0,128)]


def update_confusion_matrix(pred, label, conf_m, nb_classes, ignore = 255):

        ignore_index = label != ignore
        seg_gt = label[ignore_index].astype('int32')
        seg_pred = pred[ignore_index].astype('int32')
        index = (seg_gt * nb_classes + seg_pred).astype('int32')
        label_count = np.bincount(index)
        for i_label in range(nb_classes):
            for i_pred_label in range(nb_classes):
                cur_index = i_label * nb_classes + i_pred_label
                if cur_index < len(label_count):
                    conf_m[i_label, i_pred_label] += label_count[cur_index] #notice here, first dimension is label,second dimension is prediction.
        return conf_m


def imwrite_grid(image,label,prediction,uncertainty,border,prefix_dir, imageId):
    h,w,_ = image.shape
    grid_num = h/border
    for i in range(grid_num):
        for j in range(grid_num):
            start_i = border*i
            start_j = border*j
            end_i = border*(i+1)
            end_j = border*(j+1)
            cv2.imwrite(os.path.join(prefix_dir,"out{}_patch{}_{}.png".format(imageId,i,j)),
                        np.concatenate((image[start_i:end_i,start_j:end_j],
                                        visualize_label(label[start_i:end_i,start_j:end_j]),
                                        visualize_label(prediction[start_i:end_i,start_j:end_j]),uncertainty[start_i:end_i,start_j:end_j]), axis=1))


def pad_image(img, target_size):
    """Pad an image up to the target size."""
    rows_missing = max(target_size[0] - img.shape[0], 0)
    cols_missing = max(target_size[1] - img.shape[1], 0)
    try:
        padded_img = np.pad(img, ((0, rows_missing), (0, cols_missing), (0, 0)), 'constant')
    except  Exception as e:
        print(str(e))
        pass
    return padded_img, [0,target_size[0]-rows_missing,0,target_size[1] - cols_missing]


def pad_edge(img, target_size):
    """Pad an image up to the target size."""
    rows_missing = max(target_size[0] - img.shape[0], 0)
    cols_missing = max(target_size[1] - img.shape[1], 0)
    padded_img = np.pad(img, ((0, rows_missing), (0, cols_missing), (0, 0)), 'constant')
    return padded_img, [0,target_size[0]-rows_missing,0,target_size[1] - cols_missing]


def apply_mask(image, mask, color, alpha=0.5):
    """Apply the given mask to the image.
    """
    for c in range(3):
        image[:, :, c] = np.where(mask == 1,
                                  image[:, :, c] *
                                  (1 - alpha) + alpha * color[c] * 255,
                                  image[:, :, c])
    return image


#https://github.com/matterport/Mask_RCNN/blob/master/visualize.py
def visualize_binary_mask(image, label,color, class_num, alpha=0.5):
    """Color classes a good distance away from each other."""

    image = np.copy(image)
    for ii in range(1,class_num):# background no mask
        for c in range(3):
            image[:, :, c] = np.where(label == ii,
                                      image[:, :, c] *
                                      (1 - alpha) + alpha * color[c],
                                      image[:, :, c])
    return image


def crop_saliency(img, label):
    img_copy = np.copy(img)
    if len(label.shape) == 2:
        label = label[:,:,np.newaxis]*np.ones((1,1,3))

    img_copy[label==0] = 255 #white

    return img_copy



def visualize_label(label, class_num=21, ignore_label = 255):
    """Color classes a good distance away from each other."""
    if len(label.shape) == 3:
        label = np.squeeze(label)
    h, w = label.shape
    img_color = np.zeros((h, w, 3)).astype('uint8')
    if class_num == 2:#if two class, using white-black colormap to enlarge contrast
        my_label_colours = [(255, 255, 255),(0, 0, 0)]
    else:
        if class_num > 21:
            my_label_colours = [PALETTE_RGB[i][::-1] for i in range(class_num)]
        else:
            my_label_colours = label_colours

    for i in range(0,class_num):
            img_color[label == i] = my_label_colours[i]



    img_color[label==ignore_label] = ignore_color#highlight ignore label

    return img_color


def visualize_uncertainty(prob):
    prob = np.amax(prob,axis=2,keepdims=False)*255
    return prob

def visualize_strict_uncertainty(prob,label):
    h,w,c = prob.shape
    gt = np.reshape(label,(h*w))
    prob = np.reshape(prob,(h*w,c))
    gt_idx = np.where(gt > -1)[0]
    idx = np.vstack((gt_idx, gt))
    tmp = prob[list(idx)] #TODO advance index in numpy, here is buggy, because 255 ignore,index 255 is out of bounds for axis 1 with size 21
    tmp = tmp*255
    tmp = np.reshape(tmp,(w,h)).astype(np.uint8)
    return tmp


def visualize_mixlabel(label,mask):#H,W,C
    """Color classes a good distance away from each other."""
    h, w, c = label.shape
    img_color = np.zeros((h, w, 3)).astype('uint8')
    for i in range(h):
        for j in range(w):
            aa = 0
            bb=0
            cc=0
            if len(np.where(label[i,j]>0)[0]) >1:
                img_color[i,j] = fuzzy_color
                continue

            for k in range(c):
                if label[i, j, k] > 0:
                    aa += label[i, j, k]*label_colours[k][0]
                    bb + label[i, j, k]*label_colours[k][1]
                    cc += label[i, j, k]*label_colours[k][2]

            img_color[i,j] = [aa,bb,cc]


    img_color[mask==0] = ignore_color#highlight ignore label
    return img_color



def edge_predict_slider(full_image, edge, predictor, classes, tile_size):
    """slider is responsible for generate slide window,
    the window image may be smaller than the original image(if the original image is smaller than tile_size),
     so we need to padding.
     here we should notice that the network input is fixed.
     before send the image into the network, we should do some padding"""
    tile_size = (tile_size, tile_size)
    overlap = 1/3
    stride = ceil(tile_size[0] * (1 - overlap))
    tile_rows = int(ceil((full_image.shape[0] - tile_size[0]) / stride) + 1)  # strided convolution formula
    tile_cols = int(ceil((full_image.shape[1] - tile_size[1]) / stride) + 1)
    full_probs = np.zeros((full_image.shape[0], full_image.shape[1], classes))
    count_predictions = np.zeros((full_image.shape[0], full_image.shape[1], classes))
    tile_counter = 0
    for row in range(tile_rows):
        for col in range(tile_cols):
            x1 = int(col * stride)
            y1 = int(row * stride)
            x2 = min(x1 + tile_size[1], full_image.shape[1])
            y2 = min(y1 + tile_size[0], full_image.shape[0])
            x1 = max(int(x2 - tile_size[1]), 0)  # for portrait images the x1 underflows sometimes
            y1 = max(int(y2 - tile_size[0]), 0)  # for very few rows y1 underflows
            img = full_image[y1:y2, x1:x2]
            ed = edge[y1:y2, x1:x2]

            padded_img, padding_index = pad_image(img, tile_size) #only happen in validation or test when the original image size is already smaller than tile_size
            padded_ed, padding_ed_index = pad_image(ed, tile_size)

            tile_counter += 1
            padded_img = padded_img[None, :, :, :].astype('float32') # extend one dimension
            padded_ed = np.squeeze(padded_ed)
            padded_ed = padded_ed[None, :, :].astype('float32')  # extend one dimension

            padded_prediction = predictor(padded_img, padded_ed)[0][0]
            prediction_no_padding = padded_prediction[padding_index[0]:padding_index[1],padding_index[2]:padding_index[3],:]
            count_predictions[y1:y2, x1:x2] += 1
            full_probs[y1:y2, x1:x2] += prediction_no_padding  # accumulate the predictions also in the overlapping regions

    # average the predictions in the overlapping regions
    full_probs /= count_predictions
    return full_probs


def edge_predict_scaler(full_image, edge, predictor, scales, classes, tile_size, is_densecrf=True):
    """ scaler is only respnsible for generate multi scale input for slider """

    full_probs = np.zeros((full_image.shape[0], full_image.shape[1], classes))
    h_ori, w_ori = full_image.shape[:2]
    for scale in scales:
        scaled_img = cv2.resize(full_image, (int(scale*w_ori), int(scale*h_ori)))
        scaled_edge = cv2.resize(edge, (int(scale * w_ori), int(scale * h_ori)))#resize on single channel will make extra dim disappear!!!
        scaled_probs = edge_predict_slider(scaled_img, scaled_edge[:,:,None], predictor, classes, tile_size)
        probs = cv2.resize(scaled_probs, (w_ori,h_ori))
        full_probs += probs
    full_probs /= len(scales)
    if is_densecrf:
        full_probs = dense_crf(full_probs,sxy_bilateral=(67,67),srgb_bilateral=(3,3,3), n_iters=10)
    return full_probs




def predict_slider(full_image, predictor, classes, tile_size, overlap = 1.0/3):
    if isinstance(tile_size, int):
        tile_size = (tile_size, tile_size)
    stride = ceil(tile_size[0] * (1 - overlap))
    tile_rows = int(ceil((full_image.shape[0] - tile_size[0]) / stride) + 1)  # strided convolution formula
    tile_cols = int(ceil((full_image.shape[1] - tile_size[1]) / stride) + 1)
    full_probs = np.zeros((full_image.shape[0], full_image.shape[1], classes))
    count_predictions = np.zeros((full_image.shape[0], full_image.shape[1], classes))
    tile_counter = 0
    for row in range(tile_rows):
        for col in range(tile_cols):
            x1 = int(col * stride)
            y1 = int(row * stride)
            x2 = min(x1 + tile_size[1], full_image.shape[1])
            y2 = min(y1 + tile_size[0], full_image.shape[0])
            x1 = max(int(x2 - tile_size[1]), 0)  # for portrait images the x1 underflows sometimes
            y1 = max(int(y2 - tile_size[0]), 0)  # for very few rows y1 underflows
            img = full_image[y1:y2, x1:x2]
            padded_img, padding_index = pad_image(img, tile_size) #only happen in validation or test when the original image size is already smaller than tile_size
            tile_counter += 1
            #padded_img = padded_img[None, :, :, :].astype('float32') # extend one dimension，must remove it, maybe influence other function.
            padded_prediction = predictor(padded_img)# TODO dongzhuoyao, may be influence other function.
            prediction_no_padding = padded_prediction[padding_index[0]:padding_index[1],padding_index[2]:padding_index[3],:]
            count_predictions[y1:y2, x1:x2] += 1
            full_probs[y1:y2, x1:x2] += prediction_no_padding  # accumulate the predictions also in the overlapping regions

    # average the predictions in the overlapping regions
    try:
        full_probs /= count_predictions
    except:
        from IPython import embed
        embed()
    return full_probs

def predict_scaler(full_image, predictor, scales, classes, tile_size, is_densecrf,overlap=1.0/3):
    """  scaler is only respnsible for generate multi scale input for slider  """

    full_probs = np.zeros((full_image.shape[0], full_image.shape[1], classes))
    h_ori, w_ori = full_image.shape[:2]
    for scale in scales:
        scaled_img = cv2.resize(full_image, (int(scale*w_ori), int(scale*h_ori)))
        scaled_probs = predict_slider(scaled_img, predictor, classes, tile_size, overlap)
        probs = cv2.resize(scaled_probs, (w_ori,h_ori))
        if scaled_probs.shape[-1]==1:
            probs = probs[:,:,None]# if dimension is 1, resize will cause dimension missed, here just keep the dimension.
        full_probs += probs
    full_probs /= len(scales)
    if is_densecrf:
        full_probs = dense_crf(full_probs, sxy_bilateral=(67, 67), srgb_bilateral=(10,10,10), n_iters=10)
    return full_probs

def visualize_feat_sigmoid(img,predictor,top_k=3):
    output = predictor(img)
    img_list = [output[:,:,i] for i in range(output.shape[-1])]
    img_list = sorted(img_list, key=lambda x: np.linalg.norm(x,1), reverse=True) # sort inplace, time costing
    result = []

    def sigmoid(x, derivative=False):
        return x * (1 - x) if derivative else 1 / (1 + np.exp(-x))

    for i in range(top_k):
        current = img_list[i]
        current = sigmoid(current)*255
        current = current.astype(np.uint8)
        current = cv2.applyColorMap(current, cv2.COLORMAP_JET)#https://blog.csdn.net/loveliuzz/article/details/73648505
        result.append(current)

    return result

def visualize_feat(img,predictor):
    output = predictor(img)
    img_list = [output[:,:,i] for i in range(output.shape[-1])]
    result = []
    for i in range(len(img_list)):
        current = img_list[i]
        current = (current - np.min(current))*255/(np.max(current)-np.min(current))
        mask = current
        mask = mask[:,:,np.newaxis]* np.ones((1,1,3))
        current = current.astype(np.uint8)
        current = cv2.applyColorMap(current, cv2.COLORMAP_JET)#https://blog.csdn.net/loveliuzz/article/details/73648505
        result.append(current)
        result.append(mask)

    return result

def visualize_feat_relative(img,predictor,top_k=5):
    output = predictor(img)
    img_list = [output[:,:,i] for i in range(output.shape[-1])]
    img_list = sorted(img_list, key=lambda x: np.sum(x), reverse=True) # sort inplace, time costing

    result = []
    for i in range(top_k):
        current = img_list[i]
        current = (current - np.min(current))*255/(np.max(current)-np.min(current))
        current = current.astype(np.uint8)
        current = cv2.applyColorMap(current, cv2.COLORMAP_JET)#https://blog.csdn.net/loveliuzz/article/details/73648505
        result.append(current)

    return result


def dense_crf(probs, img=None, n_iters=10,
              sxy_gaussian=(1, 1), compat_gaussian=4,
              kernel_gaussian=dcrf.DIAG_KERNEL,
              normalisation_gaussian=dcrf.NORMALIZE_SYMMETRIC,
              sxy_bilateral=(49, 49), compat_bilateral=5,
              srgb_bilateral=(13, 13, 13),
              kernel_bilateral=dcrf.DIAG_KERNEL,
              normalisation_bilateral=dcrf.NORMALIZE_SYMMETRIC):
    """DenseCRF over unnormalised predictions.
       More details on the arguments at https://github.com/lucasb-eyer/pydensecrf.
    Args:
      probs: class probabilities per pixel.
      img: if given, the pairwise bilateral potential on raw RGB values will be computed.
      n_iters: number of iterations of MAP inference.
      sxy_gaussian: standard deviations for the location component of the colour-independent term.
      compat_gaussian: label compatibilities for the colour-independent term (can be a number, a 1D array, or a 2D array).
      kernel_gaussian: kernel precision matrix for the colour-independent term (can take values CONST_KERNEL, DIAG_KERNEL, or FULL_KERNEL).
      normalisation_gaussian: normalisation for the colour-independent term (possible values are NO_NORMALIZATION, NORMALIZE_BEFORE, NORMALIZE_AFTER, NORMALIZE_SYMMETRIC).
      sxy_bilateral: standard deviations for the location component of the colour-dependent term.
      compat_bilateral: label compatibilities for the colour-dependent term (can be a number, a 1D array, or a 2D array).
      srgb_bilateral: standard deviations for the colour component of the colour-dependent term.
      kernel_bilateral: kernel precision matrix for the colour-dependent term (can take values CONST_KERNEL, DIAG_KERNEL, or FULL_KERNEL).
      normalisation_bilateral: normalisation for the colour-dependent term (possible values are NO_NORMALIZATION, NORMALIZE_BEFORE, NORMALIZE_AFTER, NORMALIZE_SYMMETRIC).
    Returns:
      Refined predictions after MAP inference.
    """
    h, w, class_num = probs.shape

    probs = probs.transpose(2, 0, 1).copy(order='C')  # Need a contiguous array.

    d = dcrf.DenseCRF2D(w, h, class_num)  # Define DenseCRF model.
    U = -np.log(probs)  # Unary potential.
    U = U.reshape((class_num, -1)).astype(np.float32)  # Needs to be flat.
    d.setUnaryEnergy(U)
    d.addPairwiseGaussian(sxy=sxy_gaussian, compat=compat_gaussian,
                          kernel=kernel_gaussian, normalization=normalisation_gaussian)
    if img is not None:
        assert (img.shape[1:3] == (h, w)), "The image height and width must coincide with dimensions of the logits."
        d.addPairwiseBilateral(sxy=sxy_bilateral, compat=compat_bilateral,
                               kernel=kernel_bilateral, normalization=normalisation_bilateral,
                               srgb=srgb_bilateral, rgbim=img[0])
    Q = d.inference(n_iters)
    preds = np.array(Q, dtype=np.float32).reshape((class_num, h, w)).transpose(1, 2, 0)
    return preds


def predict_from_dir(mypredictor,image_dir,target_dir,CLASS_NUM,CROP_SIZE,is_densecrf, image_format="jpg",overlap=1.0/3):
    ll = os.listdir(image_dir)
    from tqdm import tqdm
    for l in tqdm(ll):
        filename = os.path.basename(l)
        extension = filename.rsplit(".")[-1]
        logger.info("process {}.".format(filename))
        src_img = cv2.imread(os.path.join(image_dir,l))
        result = predict_scaler(src_img, mypredictor, scales=[0.9,1,1.1], classes=CLASS_NUM, tile_size=CROP_SIZE, is_densecrf = is_densecrf, overlap=overlap)
        result = np.argmax(result, axis=2)
        cv2.imwrite(os.path.join(target_dir,filename),np.concatenate((src_img, visualize_label(result)), axis=1))






if __name__ == '__main__':
    label = np.array([1,1,1,0,0,0,0])
    pred = np.array([0,0,0,0,0,0,0])
    cm = np.array([[0,0],[0,0]])
    #cm = update_confusion_matrix(pred,label,cm,2)

    prob = np.random.rand(10,10,3)
    gt = np.ones((10,10,1),dtype=np.uint8)
    prob.resize((100,3))
    gt.resize((100))

    gt_idx = np.where(gt>-1)[0]
    idx = np.vstack((gt_idx,gt))
    tmp = prob[list(idx)]
    t = prob[[1,2,3],[2,0,0]]
    pass







