# Author: Tao Hu <taohu620@gmail.com>
from tensorpack.utils import logger
from tensorpack.dataflow.imgaug.base import ImageAugmentor
import numpy as np
import cv2


def RandomResizeSlight(ds,xrange=(0.9, 1.1), yrange=(0.9, 1.1),aspect_ratio_thres=0.1,minimum=(0,0)):
    image = ds[0]
    label = ds[1]

    assert aspect_ratio_thres >= 0
    if aspect_ratio_thres == 0:
        assert xrange == yrange

    def is_float(tp):
        return isinstance(tp[0], float) or isinstance(tp[1], float)

    assert is_float(xrange) == is_float(yrange), "xrange and yrange has different type!"
    _is_scale = is_float(xrange)

    def _get_augment_params(img):
        cnt = 0
        h, w = img.shape[:2]

        def get_dest_size():
            if _is_scale:
                sx = np.random.uniform(xrange[0],xrange[1],size=[])
                if aspect_ratio_thres == 0:
                    sy = sx
                else:
                    sy = np.random.uniform(yrange[0],yrange[1],size=[])
                destX = max(sx * w, minimum[0])
                destY = max(sy * h, minimum[1])
            else:
                sx = np.random.uniform(xrange[0],xrange[1],size=[])
                if aspect_ratio_thres == 0:
                    sy = sx * 1.0 / w * h
                else:
                    sy = np.random.uniform(yrange[0],yrange[1],size=[])
                destX = max(sx, minimum[0])
                destY = max(sy, minimum[1])
            return (int(destX + 0.5), int(destY + 0.5))

        while True:
            destX, destY = get_dest_size()
            if aspect_ratio_thres > 0:  # don't check when thres == 0
                oldr = w * 1.0 / h
                newr = destX * 1.0 / destY
                diff = abs(newr - oldr) / oldr
                if diff >= aspect_ratio_thres + 1e-5:
                    cnt += 1
                    if cnt > 50:
                        logger.warn("RandomResize failed to augment an image")
                        return h, w, h, w
                        break
                    continue
                return h, w, destY, destX

    h, w, destY, destX = _get_augment_params(image)
    image = cv2.resize(image, (destY, destX),interpolation=cv2.INTER_LINEAR)
    label = cv2.resize(label, (destY,destX),interpolation=cv2.INTER_NEAREST)
    return [image,label]


def RandomResize(ds,xrange=(0.7, 1.5), yrange=(0.7, 1.5),aspect_ratio_thres=0.1,minimum=(0,0)):
    image = ds[0]
    label = ds[1]

    assert aspect_ratio_thres >= 0
    if aspect_ratio_thres == 0:
        assert xrange == yrange

    def is_float(tp):
        return isinstance(tp[0], float) or isinstance(tp[1], float)

    assert is_float(xrange) == is_float(yrange), "xrange and yrange has different type!"
    _is_scale = is_float(xrange)

    def _get_augment_params(img):
        cnt = 0
        h, w = img.shape[:2]

        def get_dest_size():
            if _is_scale:
                sx = np.random.uniform(xrange[0],xrange[1],size=[])
                if aspect_ratio_thres == 0:
                    sy = sx
                else:
                    sy = np.random.uniform(yrange[0],yrange[1],size=[])
                destX = max(sx * w, minimum[0])
                destY = max(sy * h, minimum[1])
            else:
                sx = np.random.uniform(xrange[0],xrange[1],size=[])
                if aspect_ratio_thres == 0:
                    sy = sx * 1.0 / w * h
                else:
                    sy = np.random.uniform(yrange[0],yrange[1],size=[])
                destX = max(sx, minimum[0])
                destY = max(sy, minimum[1])
            return (int(destX + 0.5), int(destY + 0.5))

        while True:
            destX, destY = get_dest_size()
            if aspect_ratio_thres > 0:  # don't check when thres == 0
                oldr = w * 1.0 / h
                newr = destX * 1.0 / destY
                diff = abs(newr - oldr) / oldr
                if diff >= aspect_ratio_thres + 1e-5:
                    cnt += 1
                    if cnt > 50:
                        logger.warn("RandomResize failed to augment an image")
                        return h, w, h, w
                        break
                    continue
                return h, w, destY, destX

    h, w, destY, destX = _get_augment_params(image)
    image = cv2.resize(image, (destY, destX),interpolation=cv2.INTER_LINEAR)
    label = cv2.resize(label, (destY,destX),interpolation=cv2.INTER_NEAREST)
    return [image,label]



class RandomCropWithPadding(ImageAugmentor):
    def __init__(self, crop_size, ignore_label=255):
        super(RandomCropWithPadding, self).__init__()
        self.crop_size = crop_size
        if isinstance(crop_size,int):
            self.crop_size = (crop_size,crop_size)

        self.ignore_label = ignore_label
        self._init()

    def _get_augment_params(self, img):
        self.h0 = img.shape[0]
        self.w0 = img.shape[1]

        if self.crop_size[0] > self.h0:
            top = (self.crop_size[0] - self.h0) / 2
            bottom = (self.crop_size[0] - self.h0) - top
        else:
            top = 0
            bottom = 0

        if self.crop_size[1] > self.w0:
            left = (self.crop_size[1] - self.w0) / 2
            right = (self.crop_size[1] - self.w0) - left
        else:
            left = 0
            right = 0
        new_shape = (top + bottom + self.h0, left + right + self.w0)
        diffh = new_shape[0] - self.crop_size[0]
        assert diffh >= 0
        crop_start_h = 0 if diffh == 0 else self.rng.randint(diffh)
        diffw = new_shape[1] - self.crop_size[1]
        assert diffw >= 0
        crop_start_w = 0 if diffw == 0 else self.rng.randint(diffw)
        return (top, bottom, left, right, crop_start_h, crop_start_w)

    def _augment(self, img, param, id = 0):
        top, bottom, left, right, crop_start_h, crop_start_w = param
        if id <= 1:
            il = self.ignore_label
        else:
            il = 0

        img = cv2.copyMakeBorder(img, top, bottom, left, right, cv2.BORDER_CONSTANT, value=il)

        assert crop_start_h + self.crop_size[0] <= img.shape[0], crop_start_w + self.crop_size[1] <= img.shape[1]
        return img[crop_start_h:crop_start_h + self.crop_size[0], crop_start_w:crop_start_w + self.crop_size[1]]



