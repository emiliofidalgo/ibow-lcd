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

#include "ibow-lcd/lcdetector.h"

namespace ibow_lcd {

LCDetector::LCDetector(const LCDetectorParams& params) {
  // Creating the image index
  index_ = std::make_shared<obindex2::ImageIndex>(params.k,
                                                  params.s,
                                                  params.t,
                                                  params.merge_policy,
                                                  params.purge_descriptors);
  // Storing the remaining parameters
  p_ = params.p;
  nndr_ = params.nndr;
}

LCDetector::~LCDetector() {}

void LCDetector::process(const unsigned image_id,
                         const std::vector<cv::KeyPoint>& kps,
                         const cv::Mat& descs,
                         LCDetectorResult* result) {
  // Adding the current image to the queue to be added in the future
  queue_ids_.push(image_id);
  queue_kps_.push(kps);
  queue_descs_.push(descs);

  // Assessing if, at least, p images have arrived
  if (queue_ids_.size() < p_) {
    result->status = LC_NOT_ENOUGH_IMAGES;
    return;
  }

  // Adding new hypothesis
  unsigned newimg_id = queue_ids_.front();
  queue_ids_.pop();

  std::vector<cv::KeyPoint> newimg_kps = queue_kps_.front();
  queue_kps_.pop();

  cv::Mat newimg_descs = queue_descs_.front();
  queue_descs_.pop();

  addImage(newimg_id, newimg_kps, newimg_descs);

  // Searching similar images in the index
  // Matching the descriptors agains the current visual words
  std::vector<std::vector<cv::DMatch> > matches_feats;

  // Searching the query descriptors against the features
  index_->searchDescriptors(descs, &matches_feats, 2, 64);

  // Filtering matches according to the ratio test
  std::vector<cv::DMatch> matches;
  filterMatches(matches_feats, &matches);

  std::vector<obindex2::ImageMatch> image_matches;

  // We look for similar images according to the filtered matches found
  index_->searchImages(descs, matches, &image_matches, true);

  // Showing results
  for (int j = 0; j < std::min(5, static_cast<int>(image_matches.size()));
                                                                        j++) {
    std::cout << "Cand: " << image_matches[j].image_id <<  ", " <<
                 "Score: " << image_matches[j].score << std::endl;
  }

  // TODO(emilio): Close image is considered a correct loop
  result->status = LC_DETECTED;
  result->query_id = image_id;
  result->train_id = image_matches[0].image_id;
}

void LCDetector::addImage(const unsigned image_id,
                          const std::vector<cv::KeyPoint>& kps,
                          const cv::Mat& descs) {
  if (index_->numImages() == 0) {
    // This is the first image that is inserted into the index
    index_->addImage(image_id, kps, descs);
  } else {
    // We have to search the descriptor and filter them before adding descs
    // Matching the descriptors
    std::vector<std::vector<cv::DMatch> > matches_feats;

    // Searching the query descriptors against the features
    index_->searchDescriptors(descs, &matches_feats, 2, 64);

    // Filtering matches according to the ratio test
    std::vector<cv::DMatch> matches;
    filterMatches(matches_feats, &matches);

    // Finally, we add the image taking into account the correct matchings
    index_->addImage(image_id, kps, descs, matches);
  }
}

void LCDetector::filterMatches(
      const std::vector<std::vector<cv::DMatch> >& matches_feats,
      std::vector<cv::DMatch>* matches) {
  // Clearing the current matches vector
  matches->clear();

  // Filtering matches according to the ratio test
  for (unsigned m = 0; m < matches_feats.size(); m++) {
    if (matches_feats[m][0].distance <= matches_feats[m][1].distance * nndr_) {
      matches->push_back(matches_feats[m][0]);
    }
  }
}

}  // namespace ibow_lcd
