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

#include "ibow-lcd/bayes_filter.h"

namespace ibow_lcd {

BayesFilter::BayesFilter() :
    nimages_(0) {
  posterior_.reserve(150000);
  prior_.reserve(150000);
}

BayesFilter::~BayesFilter() {}

void BayesFilter::addImage(const unsigned image) {
  image_to_index_[image] = nimages_;
  index_to_image_[nimages_] = image;
  nimages_++;
  if (posterior_.size() == 0) {
    posterior_.push_back(1.0);
  } else {
    posterior_.push_back(0.0);
  }
}

void BayesFilter::predict() {
  prior_.clear();
  if (nimages_ > 4) {
    prior_.resize(nimages_, 0.0);

    prior_[0] += posterior_[0] * 0.33;
    prior_[1] += posterior_[0] * 0.33;
    prior_[2] += posterior_[0] * 0.33;

    prior_[0] += posterior_[1] * 0.25;
    prior_[1] += posterior_[1] * 0.25;
    prior_[2] += posterior_[1] * 0.25;
    prior_[3] += posterior_[1] * 0.25;

    for (unsigned img_id = 2; img_id < nimages_ - 2; img_id++) {
      if (nimages_ > 5) {
        prior_[img_id - 2] += posterior_[img_id] * 0.09;
        prior_[img_id - 1] += posterior_[img_id] * 0.18;
        prior_[img_id]     += posterior_[img_id] * 0.36;
        prior_[img_id + 1] += posterior_[img_id] * 0.18;
        prior_[img_id + 2] += posterior_[img_id] * 0.09;

        double prob = 0.1 / (nimages_ - 5);

        // Previous probabilites
        for (unsigned i = 0; i < img_id - 1; i++) {
          prior_[i] += posterior_[img_id] * prob;
        }

        // Post probabilities
        for (unsigned i = img_id + 2; i < nimages_; i++) {
          prior_[i] += posterior_[img_id] * prob;
        }
      } else {
        prior_[img_id - 2] += posterior_[img_id] * 0.11;
        prior_[img_id - 1] += posterior_[img_id] * 0.2;
        prior_[img_id]     += posterior_[img_id] * 0.38;
        prior_[img_id + 1] += posterior_[img_id] * 0.2;
        prior_[img_id + 2] += posterior_[img_id] * 0.11;
      }
    }

    prior_[nimages_ - 1] += posterior_[nimages_ - 2] * 0.25;
    prior_[nimages_ - 2] += posterior_[nimages_ - 2] * 0.25;
    prior_[nimages_ - 3] += posterior_[nimages_ - 2] * 0.25;
    prior_[nimages_ - 4] += posterior_[nimages_ - 2] * 0.25;

    prior_[nimages_ - 1] += posterior_[nimages_ - 1] * 0.33;
    prior_[nimages_ - 2] += posterior_[nimages_ - 1] * 0.33;
    prior_[nimages_ - 3] += posterior_[nimages_ - 1] * 0.33;
  } else {
    double prob = 1.0 / nimages_;
    prior_.resize(nimages_, prob);
  }
}

void BayesFilter::update(
  const std::vector<obindex2::ImageMatch>& image_matches) {
  // Creating a vector for mean and stdev computation
  std::vector<double> scores;
  for (unsigned cand_id = 0; cand_id < image_matches.size(); cand_id++) {
    scores.push_back(image_matches[cand_id].score);
  }

  // Mean computation.
  double sum = std::accumulate(scores.begin(), scores.end(), 0.0);
  double mean =  sum / scores.size();

  // Stdev computation.
  double accum = 0.0;
  for (unsigned score_ind = 0; score_ind < scores.size(); score_ind++) {
    accum += (scores[score_ind] - mean) * (scores[score_ind] - mean);
  }
  double stdev = sqrt(accum / (scores.size() - 1));

  // Setting the limit for scores.
  double limit = mean + stdev;

  std::vector<double> likelihood(nimages_, 1.0);
  for (unsigned cand_id = 0; cand_id < image_matches.size(); cand_id++) {
    if (image_matches[cand_id].score > limit) {
      int index = image_to_index_[image_matches[cand_id].image_id];
      likelihood[index] = (image_matches[cand_id].score - stdev) / mean;
    }
  }

  sum = 0.0;
  for (unsigned image_id = 0; image_id < nimages_; image_id++) {
    posterior_[image_id] = likelihood[image_id] * prior_[image_id];
    sum += posterior_[image_id];
  }

  // Normalizing the final result.
  for (unsigned image_id = 0; image_id < nimages_; image_id++) {
    posterior_[image_id] /= sum;
  }
}

void BayesFilter::getResults(std::vector<BayesFilterResult>* images) {
  std::vector<double> sum_scores(nimages_, 0.0);
  if (nimages_ > 5) {
    // Two initial locations
    sum_scores[0] += posterior_[0];
    sum_scores[0] += posterior_[1];
    sum_scores[0] += posterior_[2];
    BayesFilterResult img0(index_to_image_[0], sum_scores[0]);
    images->push_back(img0);

    sum_scores[1] += posterior_[0];
    sum_scores[1] += posterior_[1];
    sum_scores[1] += posterior_[2];
    sum_scores[1] += posterior_[3];
    BayesFilterResult img1(index_to_image_[1], sum_scores[1]);
    images->push_back(img1);

    // Computing the sum of scores for the rest of the images
    for (unsigned img_index = 2; img_index < nimages_ - 2; img_index++) {
      sum_scores[img_index] += posterior_[img_index - 2];
      sum_scores[img_index] += posterior_[img_index - 1];
      sum_scores[img_index] += posterior_[img_index];
      sum_scores[img_index] += posterior_[img_index + 1];
      sum_scores[img_index] += posterior_[img_index + 2];

      BayesFilterResult imgind(index_to_image_[img_index],
                               sum_scores[img_index]);
      images->push_back(imgind);
    }

    // Two last locations
    sum_scores[nimages_ - 1] += posterior_[nimages_ - 1];
    sum_scores[nimages_ - 1] += posterior_[nimages_ - 2];
    sum_scores[nimages_ - 1] += posterior_[nimages_ - 3];
    BayesFilterResult locn(index_to_image_[nimages_ - 1],
                           sum_scores[nimages_ - 1]);
    images->push_back(locn);

    sum_scores[nimages_ - 2] += posterior_[nimages_ - 1];
    sum_scores[nimages_ - 2] += posterior_[nimages_ - 2];
    sum_scores[nimages_ - 2] += posterior_[nimages_ - 3];
    sum_scores[nimages_ - 2] += posterior_[nimages_ - 4];
    BayesFilterResult locn_1(index_to_image_[nimages_ - 2],
                             sum_scores[nimages_ - 2]);
    images->push_back(locn_1);

    std::sort(images->begin(), images->end());
  }
}

}  // namespace ibow_lcd
