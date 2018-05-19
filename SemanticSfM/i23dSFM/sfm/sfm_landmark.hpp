// Copyright (c) 2015 Pierre Moulon.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef I23DSFM_SFM_LANDMARK_HPP
#define I23DSFM_SFM_LANDMARK_HPP

#include "i23dSFM/numeric/numeric.h"
#include <cereal/cereal.hpp> // Serialization

namespace i23dSFM {
namespace sfm {

/// Define 3D-2D tracking data: 3D landmark with it's 2D observations
struct Observation
{
  Observation():id_feat(UndefinedIndexT) {  }
  Observation(const Vec2 & p, IndexT idFeat): x(p), id_feat(idFeat) {}
  Observation(const Vec2 & p, IndexT idFeat, int sl): x(p), id_feat(idFeat), semantic_label(sl) {}  

  Vec2 x;
  IndexT id_feat;
  int semantic_label;


  // Serialization
  template <class Archive>
  void save( Archive & ar) const
  {
    ar(cereal::make_nvp("id_feat", id_feat ));
    const std::vector<double> pp = { x(0), x(1) };
    ar(cereal::make_nvp("x", pp));
    ar(cereal::make_nvp("semantic_label", semantic_label));    
  }

  // Serialization
  template <class Archive>
  void load( Archive & ar)
  {
    ar(cereal::make_nvp("id_feat", id_feat ));
    std::vector<double> p(2);
    ar(cereal::make_nvp("x", p));
    x = Eigen::Map<const Vec2>(&p[0]);
    ar(cereal::make_nvp("semantic_label", semantic_label));
  }
};
/// Observations are indexed by their View_id
typedef Hash_Map<IndexT, Observation> Observations;


/// Define a landmark (a 3D point, with it's 2d observations)
struct Landmark
{
  Observations obs;
  Vec3 X;
  int semantic_label = -1;

  // Serialization
  template <class Archive>
  void save( Archive & ar) const
  {
    const std::vector<double> point = { X(0), X(1), X(2) };
    ar(cereal::make_nvp("X", point ));
    ar(cereal::make_nvp("observations", obs));
    ar(cereal::make_nvp("semantic_label", semantic_label));    
  }

  template <class Archive>
  void load( Archive & ar)
  {
    std::vector<double> point(3);
    ar(cereal::make_nvp("X", point ));
    X = Eigen::Map<const Vec3>(&point[0]);
    ar(cereal::make_nvp("observations", obs));
    ar(cereal::make_nvp("semantic_label", semantic_label));    
  }
};

} // namespace sfm
} // namespace i23dSFM

#endif // I23DSFM_SFM_LANDMARK_HPP
