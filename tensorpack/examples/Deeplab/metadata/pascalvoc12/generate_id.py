# Author: Tao Hu <taohu620@gmail.com>

f = open("val.txt","r")
fid = open("val_id.txt","w")
for line in f.readlines():
    line = line.strip("\n")
    line = line.split()[0]
    line = line.replace("JPEGImages/","")
    line = line.replace(".jpg","")
    fid.write("{}\n".format(line))

fid.close()
