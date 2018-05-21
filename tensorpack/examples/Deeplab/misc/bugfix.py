# Author: Tao Hu <taohu620@gmail.com>

dir_result = "/Users/ht/Desktop/12.24compressed"

import os
tt = os.listdir(dir_result)
for name in tt:
    old_name = os.path.join(dir_result,name)
    #new_name = old_name.replace("tiff","tif")
    #new_name = old_name.replace("nnsbruck", "innsbruck")
    new_name = old_name.replace("yrol", "tyrol")
    os.rename(old_name,new_name)