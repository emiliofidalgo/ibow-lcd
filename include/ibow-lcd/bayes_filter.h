/**
* This file is part of ibow-lcd.
*
* Copyright (C) 2017 Emilio Garcia-Fidalgo <emilio.garcia@uib.es> (University of the Balearic Islands)
*
* ibow-lcd is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ibow-lcd is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ibow-lcd. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INCLUDE_IBOW_LCD_BAYES_FILTER_H_
#define INCLUDE_IBOW_LCD_BAYES_FILTER_H_

#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <numeric>
#include <utility>
#include <vector>

#include "obindex2/binary_index.h"

namespace ibow_lcd {

class BayesFilterResult {
 public:
  BayesFilterResult() {}

  BayesFilterResult(const int img_id, const float sum_prob) {
    entry_ = std::make_pair(img_id, sum_prob);
  }

  inline int getImageId() {
    return entry_.first;
  }

  inline double getSumProbs() {
    return entry_.second;
  }

  inline bool operator<(const BayesFilterResult& llre) const {
    return entry_.second > llre.entry_.second;
  }

  std::pair<int, float> entry_;
  std::vector<cv::DMatch> matches_;
};

class BayesFilter {
 public:
  BayesFilter();
  virtual ~BayesFilter();

  void addImage(const unsigned image);
  void predict();
  void update(const std::vector<obindex2::ImageMatch>& image_matches);
  void getResults(std::vector<BayesFilterResult>* images);

 private:
  unsigned nimages_;
  std::vector<double> posterior_;
  std::vector<double> prior_;
  //std::unordered_map<int, std::vector<cv::DMatch> > _last_matches;
  std::unordered_map<unsigned, unsigned> index_to_image_;
  std::unordered_map<unsigned, unsigned> image_to_index_;
};

}  // namespace ibow_lcd

#endif  // INCLUDE_IBOW_LCD_BAYES_FILTER_H_
