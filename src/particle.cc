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

#include "ibow-lcd/particle.h"

namespace ibow_lcd {

Particle::Particle() :
  image_index_(0),
  weight_(0.0f),
  norm_weight_(0.0f) {
}

void Particle::move() {
}

void Particle::evaluate(const std::unordered_map<int, double>& scores) {
}

double Particle::randomGauss(double variance) {
  if (variance < 0) {
    variance = -variance;
  }

  double x1, x2, w, y1;

  do {
    x1 = (2.0f * rand() / RAND_MAX) - 1.0;
    x2 = (2.0f * rand() / RAND_MAX) - 1.0;
    w = (x1 * x1) + (x2 * x2);
  } while ( w >= 1.0 );

  w = sqrt((-2.0 * log(w)) / w);
  y1 = x1 * w;
  return (sqrt(variance) * y1);
}

}  // namespace ibow_lcd
