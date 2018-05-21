# Author: Tao Hu <taohu620@gmail.com>

f = open("list.txt","r")

ff = open("train.txt","w")
ff_val = open("val.txt","w")


lines = f.readlines()

train_lines = lines[:230]
val_lines = lines[230:]

for l in train_lines:
    ii = l.replace("png","JPG")
    ii = ii.replace("seg_","")
    ii = ii.replace("\n","")
    ff.write("src/{} gt/{}".format(ii,l))
ff.close()


for l in val_lines:
    ii = l.replace("png","JPG")
    ii = ii.replace("seg_","")
    ii = ii.replace("\n", "")
    ff_val.write("src/{} gt/{}".format(ii,l))
ff_val.close()


