# DIP-i23d
Our own 3d reconstruction project of Digital Image Processing course in Peking University.

**Code for other usages are not allowed!**

## Linux Compilation of SemanticSfM

**1. Install Dependencies**
* sudo apt-get install libpng-dev libjpeg-dev libtiff-dev libxxf86vm1 libxxf86vm-dev libxi-dev libxrandr-dev

* install mpi-3.2

**2. Build SemanticSfM**
```
 $ cd SemanticSfM
 $ mkdir build && cd build
 $ cmake ..
 ```

Compile the project
 * $ make

For a multi-core compilation (Replace NBcore with the number of threads)
 * $ make -j NBcore

For test if build successfully
 * $ make test

**3. Run SemanticSfM**

 ```
$ cd SemanticSfM
$ ./do.sh ${image_dir} ${semantic_image_dir} ${output_dir}
 ```

 ## About this Project

