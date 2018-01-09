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

#include <algorithm>
#include <queue>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "ibow-lcd/bayes_filter.h"
#include "obindex2/binary_index.h"

namespace ibow_lcd {

// Island
struct Island {
  explicit Island(unsigned image_id,
                  double sc,
                  unsigned min_img,
                  unsigned max_img) :
        min_img_id(min_img),
        max_img_id(max_img),
        img_id(image_id),
        score(sc) {}

  unsigned size() {
    return max_img_id - min_img_id + 1;
  }

  bool fits(unsigned image_id) {
    bool response = false;
    if (image_id >= min_img_id && image_id <= max_img_id) {
      response = true;
    }
    return response;
  }

  bool overlaps(const Island& island) const {
    unsigned a1 = min_img_id;
    unsigned a2 = max_img_id;
    unsigned b1 = island.min_img_id;
    unsigned b2 = island.max_img_id;

    return (b1 <= a1 && a1 <= b2) || (a1 <= b1 && b1 <= a2);
  }

  void adjustLimits(const unsigned image_id, unsigned* min, unsigned* max) {
    // If the image is to the right of the island
    if (image_id > max_img_id) {
      if (*min <= max_img_id) {
        *min = max_img_id + 1;
      }
    } else {
      // Otherwise, the image is to the left of the island
      if (*max >= min_img_id) {
        *max = min_img_id - 1;
      }
    }
  }

  void incrementScore(double sc) {
    score += sc;
  }

  void normalizeScore() {
    score = score / size();
  }

  std::string toString() const {
    std::stringstream ss;
    ss << "[" << min_img_id << " - " << max_img_id << "] Score: " << score
       << " | Img Id: " << img_id << std::endl;
    return ss.str();
  }

  bool operator<(const Island& island) const { return score > island.score; }

  unsigned min_img_id;
  unsigned max_img_id;
  unsigned img_id;
  double score;
};

// LCDetectorParams
struct LCDetectorParams {
  LCDetectorParams() :
    k(16),
    s(150),
    t(4),
    merge_policy(obindex2::MERGE_POLICY_NONE),
    purge_descriptors(true),
    min_feat_apps(2),
    p(250),
    nndr(0.8),
    min_score(0.3),
    island_size(7),
    min_inliers(22),
    nframes_after_lc(3),
    min_consecutive_loops(0) {}

  // Image index params
  unsigned k;  // Branching factor for the image index
  unsigned s;  // Maximum leaf size for the image index
  unsigned t;  // Number of trees to search in parallel
  obindex2::MergePolicy merge_policy;  // Merging policy
  bool purge_descriptors;  // Delete descriptors from index?
  unsigned min_feat_apps;  // Min apps of a feature to be a visual word

  // Loop Closure Params
  unsigned p;  // Previous images to be discarded when searching for a loop
  float nndr;  // Nearest neighbour distance ratio
  double min_score;  // Min score to consider an image matching as correct
  unsigned island_size;  // Max number of images of an island
  unsigned min_inliers;  // Minimum number of inliers to consider a loop
  unsigned nframes_after_lc;  // Number of frames after a lc to wait for new lc
  int min_consecutive_loops;  // Min consecutive loops to avoid ep. geometry
};

// LCDetectorStatus
enum LCDetectorStatus {
  LC_DETECTED,
  LC_NOT_DETECTED,
  LC_NOT_ENOUGH_IMAGES,
  LC_NOT_ENOUGH_ISLANDS,
  LC_NOT_ENOUGH_INLIERS,
  LC_TRANSITION
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
  unsigned inliers;
};

class LCDetector {
 public:
  explicit LCDetector(const LCDetectorParams& params);
  virtual ~LCDetector();

  void process(const unsigned image_id,
               const std::vector<cv::KeyPoint>& kps,
               const cv::Mat& descs,
               LCDetectorResult* result);

 private:
  // Parameters
  unsigned p_;
  float nndr_;
  double min_score_;
  unsigned island_size_;
  unsigned island_offset_;
  unsigned min_inliers_;
  unsigned nframes_after_lc_;

  // Last loop closure detected
  LCDetectorResult last_lc_result_;
  Island last_lc_island_;
  int min_consecutive_loops_;
  int consecutive_loops_;

  // Image Index
  std::shared_ptr<obindex2::ImageIndex> index_;

  // Bayes filter
  BayesFilter bfilter;

  // Queues to delay the publication of hypothesis
  std::queue<unsigned> queue_ids_;
  std::queue<std::vector<cv::KeyPoint> > queue_kps_;
  std::queue<cv::Mat> queue_descs_;

  std::vector<std::vector<cv::KeyPoint> > prev_kps_;
  std::vector<cv::Mat> prev_descs_;

  void addImage(const unsigned image_id,
                const std::vector<cv::KeyPoint>& kps,
                const cv::Mat& descs);
  void filterMatches(
      const std::vector<std::vector<cv::DMatch> >& matches_feats,
      std::vector<cv::DMatch>* matches);
  void filterCandidates(
      const std::vector<obindex2::ImageMatch>& image_matches,
      std::vector<obindex2::ImageMatch>* image_matches_filt);
  void buildIslands(
      const std::vector<obindex2::ImageMatch>& image_matches,
      std::vector<Island>* islands);
  void getPriorIslands(
      const Island& island,
      const std::vector<Island>& islands,
      std::vector<Island>* p_islands);
  unsigned checkEpipolarGeometry(
      const std::vector<cv::Point2f>& query,
      const std::vector<cv::Point2f>& train);
  void ratioMatchingBF(const cv::Mat& query,
                     const cv::Mat& train,
                     std::vector<cv::DMatch>* matches);
  void convertPoints(const std::vector<cv::KeyPoint>& query_kps,
                     const std::vector<cv::KeyPoint>& train_kps,
                     const std::vector<cv::DMatch>& matches,
                     std::vector<cv::Point2f>* query,
                     std::vector<cv::Point2f>* train);
};

}  // namespace ibow_lcd

#endif  // INCLUDE_IBOW_LCD_LCDETECTOR_H_
