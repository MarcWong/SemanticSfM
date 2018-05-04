
// Copyright (c) 2015 Pierre MOULON.

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "i23dSFM/sfm/sfm_data.hpp"
#include "i23dSFM/sfm/sfm_data_io.hpp"

#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>

#include "third_party/stlplus3/filesystemSimplified/file_system.hpp"

using namespace i23dSFM;
using namespace i23dSFM::sfm;

struct Document
{
  SfM_Data _sfm_data;

  bool loadData(const std::string & sfm_data_ProjectPath)
  {
    return Load(_sfm_data, sfm_data_ProjectPath, ESfM_Data(ALL));
  }

  bool saveData(const std::string & sFileName)
  {
    return Save(_sfm_data, sFileName, ESfM_Data(ALL));
  }
};

