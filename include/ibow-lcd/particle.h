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
#include <cstdlib>

#include <unordered_map>

namespace ibow_lcd {

class Particle {
 public:
  // Constructor
  Particle();
  void move();
  void evaluate(const std::unordered_map<int, double>& scores);

  inline unsigned getImageIndex() {
    return image_index_;
  }

  inline float getWeight() {
    return weight_;
  }

  inline float getNormWeight() {
    return norm_weight_;
  }

  inline bool operator <(const Particle& other) const {
    return weight_ > other.weight_;
  }

 private:
  // Parameters
  unsigned image_index_;
  float weight_;
  float norm_weight_;

  static double randomGauss(double variance);
};

}  // namespace ibow_lcd

#endif  // INCLUDE_IBOW_LCD_PARTICLE_H_
