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

#include "ibow-lcd/particle_filter.h"

namespace ibow_lcd {

ParticleFilter::ParticleFilter(unsigned particles, unsigned island_offset) :
    num_particles_(particles),
    num_obs_(0),
    island_offset_(island_offset),
    neff_(static_cast<float>(num_particles_)),
    res_wheel_(particles, 0.0f),
    best_part_(0),
    best_weight_(0.0f),
    total_weight_(0.0f),
    init_(false) {
  parts_ = new Particle[num_particles_];
  new_parts_ = new Particle[num_particles_];
}

ParticleFilter::~ParticleFilter() {
  delete[] parts_;
  delete[] new_parts_;
}

void ParticleFilter::init() {
  float weight_norm = 1.0f / num_particles_;
  for (unsigned i = 0; i < num_particles_; i++) {
    parts_[i].randomize(num_obs_, island_offset_);
    parts_[i].weight = 1.0f;
    parts_[i].weight_norm = weight_norm;
  }
  neff_ = static_cast<float>(num_particles_);
}

void ParticleFilter::filter(const std::vector<Island>& islands) {
  num_obs_++;
  if (!init_) {
    init();
    init_ = true;
  } else {
    resample();
    randomize();
    moveParticles();
    clearWeights();
    evaluateParticles(islands);
    normalizeWeights();
  }
}

Particle ParticleFilter::getBestParticle() {
  return parts_[best_part_];
}

void ParticleFilter::moveParticles() {
  for (unsigned i = 0; i < num_particles_; i++) {
    parts_[i].move(num_obs_);
  }
}

void ParticleFilter::evaluateParticles(const std::vector<Island>& islands) {
  int nislands = std::min(static_cast<int>(islands.size()), 20);
  best_part_ = 0;
  best_weight_ = 0.0f;
  total_weight_ = 0.0f;
  for (int i = 0; i < nislands; i++) {
    Island tisland = islands[i];
    for (unsigned j = 0; j < num_particles_; j++) {
      float weight = parts_[j].evaluate(tisland);
      total_weight_ += weight;
      if (parts_[j].weight > best_weight_) {
        best_weight_ = parts_[j].weight;
        best_part_ = j;
      }
    }
  }
}

void ParticleFilter::normalizeWeights() {
  // Setting the normalization factor
  float factor;
  bool there_is_weight;
  if (total_weight_ > 0.0f) {
    factor = 1.0f / total_weight_;
    there_is_weight = true;
  } else {
    factor = 1.0f / num_particles_;
    there_is_weight = false;
  }

  float sum = 0.0f;

  // Setting the normalized weights
  for (unsigned i = 0; i < num_particles_; i++) {
    if (there_is_weight) {
      parts_[i].weight_norm = parts_[i].weight * factor;
    } else {
      parts_[i].weight_norm = factor;
    }

    sum += parts_[i].weight_norm * parts_[i].weight_norm;

    // Updating the resampling wheel
    if (i == 0) {
      res_wheel_[0] = parts_[0].weight_norm;
    } else {
      res_wheel_[i] = res_wheel_[i - 1] + parts_[i].weight_norm;
    }
  }

  // Updating the number of effective particles
  if (sum > 0.0f) {
    neff_ = 1.0f / sum;
  } else {
    neff_ = 0.0f;
  }
}

void ParticleFilter::resample() {
  float rand_val = Random::get(0.0f, 1.0f);
  float step = 1.0f / num_particles_;
  for (unsigned i = 0; i < num_particles_; i++) {
    Particle p = getParticleByWeight(rand_val);
    p.weight = step;
    new_parts_[i] = p;

    rand_val += step;
    if (rand_val > 1.0f) {
      rand_val -= 1.0f;
    }
  }

  // Changing the pointer for pointing to the new set
  Particle* aux = new_parts_;
  new_parts_ = parts_;
  parts_ = aux;
}

void ParticleFilter::clearWeights() {
  for (unsigned i = 0; i < num_particles_; i++) {
    parts_[i].weight = 0.0f;
  }
}

Particle ParticleFilter::getParticleByWeight(float weight) {
  for (unsigned i = 0; i < num_particles_; i++) {
    if (weight < res_wheel_[i]) {
      return parts_[i];
    }
  }
  return parts_[0];  // FIXME
}

void ParticleFilter::randomize(const float alpha) {
  int nparts = static_cast<int>(num_particles_ * alpha);
  for (int i = 0; i < nparts; i++) {
    // Selecting a particle randomly
    int part = Random::get(0, static_cast<int>(num_particles_) - 1);
    // Randomizing the corresponding particle
    parts_[part].randomize(num_obs_, island_offset_);
  }
}

}  // namespace ibow_lcd
