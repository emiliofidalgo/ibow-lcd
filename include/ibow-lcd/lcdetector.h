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

#ifndef INCLUDE_IBOW_LCD_LCDETECTOR_H_
#define INCLUDE_IBOW_LCD_LCDETECTOR_H_

#include <queue>
#include <memory>
#include <vector>

#include "obindex2/binary_index.h"

namespace ibow_lcd {

// LCDetectorParams
struct LCDetectorParams {
  LCDetectorParams() :
    k(16),
    s(150),
    t(4),
    merge_policy(obindex2::MERGE_POLICY_AND),
    purge_descriptors(true),
    p(10),
    nndr(0.8f) {}

  // Image index params
  unsigned k;  // Branching factor for the image index
  unsigned s;  // Maximum leaf size for the image index
  unsigned t;  // Number of trees to search in parallel
  obindex2::MergePolicy merge_policy;  // Merging policy
  bool purge_descriptors;

  // Loop Closure Params
  unsigned p;  // Previous images to be discarded when searching for a loop
  float nndr;  // Nearest neighbour distance ratio
};

// LCDetectorStatus
enum LCDetectorStatus {
  LC_DETECTED,
  LC_NOT_DETECTED,
  LC_NOT_ENOUGH_IMAGES
};

// LCDetectorResult
struct LCDetectorResult {
  LCDetectorResult() :
    status(LC_NOT_DETECTED),
    query_id(1),
    train_id(-1) {}

  inline bool isLoop() {
    return status == LC_DETECTED;
  }

  LCDetectorStatus status;
  unsigned query_id;
  unsigned train_id;
};

class LCDetector {
 public:
  explicit LCDetector(const LCDetectorParams& params);
  virtual ~LCDetector();

  LCDetectorResult process(const unsigned image_id,
                           const std::vector<cv::KeyPoint>& kps,
                           const cv::Mat& descs);

 private:
  // Parameters
  unsigned p_;
  unsigned nndr_;

  // Image Index
  std::shared_ptr<obindex2::ImageIndex> index_;

  // Queues to delay the publication of hypothesis
  std::queue<unsigned> queue_ids_;
  std::queue<std::vector<cv::KeyPoint> > queue_kps_;
  std::queue<cv::Mat> queue_descs_;

  void addImage(const unsigned image_id,
                const std::vector<cv::KeyPoint>& kps,
                const cv::Mat& descs);
  void filterMatches(
      const std::vector<std::vector<cv::DMatch> >& matches_feats,
      std::vector<cv::DMatch>* matches);
};

}  // namespace ibow_lcd

#endif  // INCLUDE_IBOW_LCD_LCDETECTOR_H_
