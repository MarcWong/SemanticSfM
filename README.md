# SemanticSfM

This repository contains the reference code for sparse semantic reconstruction process based on the following paper: [**Large-Scale Structure from Motion with Semantic Constraints of Aerial Images**](https://link.springer.com/chapter/10.1007/978-3-030-03398-9_30)


The PDF of the article is available at this [link](https://link.springer.com/content/pdf/10.1007%2F978-3-030-03398-9_30.pdf).

|Code Contributor|Contribution|
|---|---|
| Yu Chen(陈煜) |Structure from Motion|
| Yao Wang(王尧) |Semantic Segmentation|
|Xupu Wang(王旭普)|Data Visualization|

**Code for other usages are not allowed!**

## code map

```
$Root Directory
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
