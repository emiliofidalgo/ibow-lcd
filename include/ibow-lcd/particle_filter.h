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

#ifndef INCLUDE_IBOW_LCD_PARTICLE_FILTER_H_
#define INCLUDE_IBOW_LCD_PARTICLE_FILTER_H_

#include <string>
#include <vector>

#include "ibow-lcd/particle.h"
#include "random.hpp"

using Random = effolkronium::random_static;

namespace ibow_lcd {

class ParticleFilter {
 public:
  // Constructor
  explicit ParticleFilter(unsigned particles = 150,
                          unsigned island_offset = 5);
  virtual ~ParticleFilter();

  // Methods
  void init();
  void process(const std::vector<Island>& islands);

  inline std::string toString() const {
    std::stringstream ss;
    for (unsigned i = 0; i < num_particles_; i++) {
      if (i == best_part_) {
        ss << "* ";
      }
      ss << parts_[i].toString();
    }
    return ss.str();
  }


 private:
  unsigned num_particles_;
  int num_obs_;
  unsigned island_offset_;
  Particle* parts_;
  Particle* new_parts_;
  std::vector<float> res_wheel_;
  unsigned best_part_;
  float best_weight_;
  float total_weight_;
  bool init_;

  void moveParticles();
  void evaluateParticles(const std::vector<Island>& islands);
  void normalizeWeights();
  void resample();
  Particle getParticleByWeight(float weight);
};

}  // namespace ibow_lcd

#endif  // INCLUDE_IBOW_LCD_PARTICLE_FILTER_H_
