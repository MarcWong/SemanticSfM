# DIP-i23d
Our own 3d reconstruction project of Digital Image Processing course in Peking University.

|Author|
|------|
| 陈煜 |
|王旭普|
| 王尧 |

**Code for other usages are not allowed!**

## code map

```
$DIP-i23d
│
│─ README.md —— this file
│
|─ SemanticSfM —— structure from motion system
│  │
│  │─ do.sh  —— running script
│  │
│  │─ cmakeFindModules
│  │
│  │─ dependencies
│  │
│  │─ i23dSFM
│  │
│  │─ nonFree
│  │
│  │─ software
│  │
│  │─ testing
│  │
│  └─ third_party
│   
|─ testapp —— point cloud visualization by webGL
│  │
│  |─README.md —— user instruction of nodeJS
│  │
│  │─bin
│  │
│  │─public
│  │
│  │─routes
│  │
│  └─views
│   
└─ tensorpack —— semantic segmentation code
   │
   |─ README.md —— user instruction of tensorpack
   │
   │─ examples
   │  │
   │  └─ Deeplab
   │     │
   │     │─ metadata —— data path txt here
   │     │
   │     │─ PSSD
   │     │  │
   │     │  │- deeplabv2res101.pssd_train.py
   │     │  │
   │     │  │- deeplabv2res101.pssd_test.py
   │     │  │
   │     │  │- deeplabv2res101.pssd_val.py
   │     │  │
   │     │  ...
   │     ...
   ...
```
