import argparse

import numpy as np
import cv2
import math
import sys
#import ipdb
#sys.path.insert(0, '../../../data/COCO/')
#from COCOAllJoints import COCOJoints


nr_skeleton = 16

colors = np.random.randint( 0, 256, (nr_skeleton, 3) )


def paint_pixel( img, x, y, c, ratio ):
    img[ x, y, 0 ] = int( c[ 0 ] * ratio )
    img[ x, y, 1 ] = int( c[ 1 ] * ratio )
    img[ x, y, 2 ] = int( c[ 2 ] * ratio )

def draw_skeleton(canvas, sk):
    for j in range(nr_skeleton):
        cv2.circle(canvas, tuple(sk[j]), 2, tuple((255, 0, 0)), 2)
    cv2.line(canvas, tuple(sk[0]), tuple(sk[1]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[1]), tuple(sk[2]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[2]), tuple(sk[6]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[6]), tuple(sk[3]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[3]), tuple(sk[4]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[4]), tuple(sk[5]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[6]), tuple(sk[7]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[7]), tuple(sk[8]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[8]), tuple(sk[9]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[8]), tuple(sk[12]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[12]), tuple(sk[11]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[11]), tuple(sk[10]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[8]), tuple(sk[13]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[13]), tuple(sk[14]), (0, 255, 255), 2)
    cv2.line(canvas, tuple(sk[14]), tuple(sk[15]), (0, 255, 255), 2)
    return canvas

def draw_skeleton_beatiful(canvas,sk): # sk shape: (16,2)
    stickwidth = 3
    #stickwidth = 7
    limbSeq = [
        [0,1],
        [1,2],
        [2,6],
        [6,3],
        [3,4],
        [4,5],
        [6,7],
        [7,8],
        [8,9],
        [8,12],
        [12,11],
        [11,10],
        [8,13],
        [13,14],
        [14,15]
    ]
    # visualize
    colors = [ [0, 170, 255], [0, 85, 255], [0, 0, 255], [85, 0, 255], \
              [170, 0, 255], [255, 0, 255], [255, 0, 170], [255, 0, 85],[255, 0, 0], [255, 85, 0], [255, 170, 0], [255, 255, 0], [170, 255, 0], [85, 255, 0], [0, 255, 0], \
              [0, 255, 85], [0, 255, 170], [0, 255, 255]]
    # subset is all instances of each image
    # candidate is all points' info of each image ,include(x,y,point_score,point_id)
    cur_canvas = canvas.copy()
    for i in range(len(limbSeq)):
        #for n in range(len(subset)):
            index = np.array(limbSeq[i])
            if sk[index[0],0] == -1 or sk[index[1],0] == -1:
                 continue
            Y = sk[index.astype(int), 0]
            X = sk[index.astype(int), 1]
            mX = np.mean(X)
            mY = np.mean(Y)
            length = ((X[0] - X[1]) ** 2 + (Y[0] - Y[1]) ** 2) ** 0.5
            angle = math.degrees(math.atan2(X[0] - X[1], Y[0] - Y[1]))
            polygon = cv2.ellipse2Poly((int(mY), int(mX)), (int(length / 2), stickwidth), int(angle), 0, 360, 1)
            cv2.fillConvexPoly(cur_canvas, polygon, colors[i])

    canvas = cv2.addWeighted(canvas, 0.4, cur_canvas, 0.6, 0)
    for j in range(nr_skeleton):
        #cv2.circle(canvas, tuple(sk[j]), 2, tuple((255, 0, 0)), 2)
        cv2.circle( canvas, tuple(sk[j]), 6, tuple((255,0,0)), 2 )

    return canvas

