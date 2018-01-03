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

#ifndef INCLUDE_IBOW_LCD_PARTICLE_H_
#define INCLUDE_IBOW_LCD_PARTICLE_H_

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <unordered_map>

#include "ibow-lcd/island.h"
#include "random.hpp"

using Random = effolkronium::random_static;

namespace ibow_lcd {

class Particle {
 public:
  // Constructor
  Particle();

  // Methods
  void move(const int nimages);
  float evaluate(const Island& tisland);
  void randomize(const int nimages, const unsigned island_offset = 5);
  void clearWeights();

  inline std::string toString() const {
    std::stringstream ss;
    ss << "[" << island.min_img_id << " - " << island.max_img_id <<
          "] | Img Id: " << island.img_id << " | Weight: " << weight_norm <<
          std::endl;
    return ss.str();
  }

  bool operator<(const Particle& other) const { return weight > other.weight; }

  // Members
  Island island;
  float weight;
  float weight_norm;

 private:
  unsigned generateRandomMovement();
};

}  // namespace ibow_lcd

#endif  // INCLUDE_IBOW_LCD_PARTICLE_H_
