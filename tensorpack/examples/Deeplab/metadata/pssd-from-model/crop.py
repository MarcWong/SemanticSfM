# Author: Tao Hu <taohu620@gmail.com>
import glob
import os,cv2
from tqdm import tqdm
src_base_dir = "/data1/dataset/m1-from-model"
src_img_dir = os.path.join(src_base_dir, "m1")
src_gt_dir = os.path.join(src_base_dir,"m1-seg-result-from-model")

target_base_dir = "/data1/dataset/m1-from-model"




src_img_dir_list = glob.glob(os.path.join(src_img_dir,"*.JPG"))
src_img_dir_list.sort()

src_gt_dir_list = glob.glob(os.path.join(src_gt_dir,"*.png"))
src_gt_dir_list.sort()
border=1000


def process(img_list, gt_list):
    f_train = open("train.txt","w")
    f_val = open("val.txt","w")

    for img_path,gt_path in tqdm(zip(img_list[:250], gt_list[:250])):
        base_name = os.path.splitext(os.path.basename(img_path))[0]
        img = cv2.imread(img_path)
        gt = cv2.imread(gt_path)

        h, w, _ = img.shape
        assert h==3000, w==4000
        h_grid_num = h / border
        w_grid_num = w / border
        for i in range(h_grid_num):
            for j in range(w_grid_num):
                start_i = border * i
                start_j = border * j
                end_i = border * (i + 1)
                end_j = border * (j + 1)
                cv2.imwrite(os.path.join(target_base_dir,"train/src" ,"{}_{}_{}.jpg".format(base_name, i, j)),
                            img[start_i:end_i, start_j:end_j])
                cv2.imwrite(os.path.join(target_base_dir, "train/gt","{}_{}_{}.png".format(base_name, i, j)),
                            gt[start_i:end_i, start_j:end_j])

                f_train.write("{} {}\n".format(os.path.join("train/src", "{}_{}_{}.jpg".format(base_name, i, j))
                                                      ,os.path.join("train/gt", "{}_{}_{}.png".format(base_name, i, j))))

    for img_path,gt_path in tqdm(zip(img_list[250:], gt_list[250:])):
        base_name = os.path.splitext(os.path.basename(img_path))[0]
        img = cv2.imread(img_path)
        gt = cv2.imread(gt_path)

        h, w, _ = img.shape
        assert h==3000, w==4000
        h_grid_num = h / border
        w_grid_num = w / border
        for i in range(h_grid_num):
            for j in range(w_grid_num):
                start_i = border * i
                start_j = border * j
                end_i = border * (i + 1)
                end_j = border * (j + 1)
                cv2.imwrite(os.path.join(target_base_dir,"val/src" ,"{}_{}_{}.jpg".format(base_name, i, j)),
                            img[start_i:end_i, start_j:end_j])
                cv2.imwrite(os.path.join(target_base_dir, "val/gt","{}_{}_{}.png".format(base_name, i, j)),
                            gt[start_i:end_i, start_j:end_j])

                f_val.write("{} {}\n".format(os.path.join("val/src", "{}_{}_{}.jpg".format(base_name, i, j))
                                                      ,os.path.join("val/gt", "{}_{}_{}.png".format(base_name, i, j))))






process(src_img_dir_list, src_gt_dir_list)


