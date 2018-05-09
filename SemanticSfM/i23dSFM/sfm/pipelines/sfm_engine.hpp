
// Copyright (c) 2015 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "i23dSFM/image/image.hpp"

#include "i23dSFM/sfm/sfm_data.hpp"
#include <string>

using namespace std;
using namespace i23dSFM::image;


namespace i23dSFM {
namespace sfm {

/// Basic Reconstruction Engine.
/// Process Function handle the reconstruction.
class ReconstructionEngine
{
public:

  ReconstructionEngine(
    const SfM_Data & sfm_data,
    const std::string & soutDirectory)
    :_sOutDirectory(soutDirectory),
    _sfm_data(sfm_data),
    _bFixedIntrinsics(false)
  {
  }

  virtual ~ReconstructionEngine() {}

  virtual bool Process() = 0;

  bool Get_bFixedIntrinsics() const {return _bFixedIntrinsics;}
  void Set_bFixedIntrinsics(bool bVal) {_bFixedIntrinsics = bVal;}

  const SfM_Data & Get_SfM_Data() const {return _sfm_data;}

  void Extract_Feature_Label()
  {
    Hash_Map<IndexT, Landmark>::iterator ite;
    for(ite = _sfm_data.structure.begin(); ite != _sfm_data.structure.end(); ite++)
    {
      Observations obs = ite->second.obs;
      Hash_Map<IndexT, Observation>::iterator obsIte = obs.begin();
      IndexT viewId = obsIte->first;
      Vec2 feat_coord = obsIte->second.x;
      // load semantic images
      string semantic_img_path;

      std::shared_ptr<View> fView = _sfm_data.views.find(viewId)->second;
      semantic_img_path = _sfm_data.s_seg_root_path + fView->semantic_img_path;

      Image<unsigned char> semanticImgGray;
      if(!ReadImage(semantic_img_path.c_str(), &semanticImgGray))
      {
        cout << "cannot read semantic segmentation file: " << semantic_img_path << endl;      
        return;
      }
      ite->second.semantic_label = semanticImgGray(feat_coord[1], feat_coord[0]);
      cout << "semantic label: " << ite->second.semantic_label << endl;
    }
  }

protected:
  std::string _sOutDirectory; // Output path where outputs will be stored

  //-----
  //-- Reconstruction data
  //-----
  SfM_Data _sfm_data; // internal SfM_Data

  //-----
  //-- Reconstruction parameters
  //-----
  bool _bFixedIntrinsics;
};

} // namespace sfm
} // namespace i23dSFM
