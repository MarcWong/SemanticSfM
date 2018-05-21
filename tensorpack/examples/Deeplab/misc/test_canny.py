# Author: Tao Hu <taohu620@gmail.com>
import cv2
import numpy as np
src = "/data_a/dataset/pascalvoc2012/VOC2012trainval/VOCdevkit/VOC2012/JPEGImages/2007_000032.jpg"
img = cv2.imread(src)
edge = cv2.Canny(img, 100, 200).astype("float32")/255
edge = cv2.GaussianBlur(edge,(5,5),0)
edge = edge[:,:,None]
padded_img = np.pad(edge, ((0, 0), (0, 0), (0, 0)), 'constant')
cv2.imwrite("edge.jpg",edge*255)
