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
  island(0, 0.0, 0, 0),
  weight(0.0f),
  weight_norm(0.0f) {
}

void Particle::move(const int nimages) {
  // Moving the image index
  unsigned disp_img_index = generateRandomMovement();
  island.img_id += disp_img_index;
  if (island.img_id > static_cast<unsigned>(nimages - 1)) {
    island.img_id = nimages - 1;
  }

  // Moving the left index
  unsigned disp_left_index = generateRandomMovement();
  island.min_img_id += disp_left_index;
  if (island.min_img_id > static_cast<unsigned>(nimages - 1)) {
    island.min_img_id = nimages - 1;
  }

  // Moving the right index
  unsigned disp_right_index = generateRandomMovement();
  island.max_img_id += disp_right_index;
  if (island.max_img_id > static_cast<unsigned>(nimages - 1)) {
    island.max_img_id = nimages - 1;
  }

  // Clearing the particle weights
  clearWeights();
}

float Particle::evaluate(const Island& tisland) {
  float response = 0.0f;
  if (tisland.overlap(island)) {
    weight += tisland.score;
    response = tisland.score;
  }
  return response;
}

void Particle::randomize(const int nimages, const unsigned island_offset) {
  // Selecting a new random image
  int img_index = Random::get(0, nimages - 1);

  // Compute the limits
  // Left
  int left_lim = img_index - island_offset;
  if (left_lim < 0) {
    left_lim = 0;
  }

  // Right
  int right_lim = img_index + island_offset;
  if (right_lim > nimages - 1) {
    right_lim = nimages - 1;
  }

  // Updating the island
  island.min_img_id = static_cast<unsigned>(left_lim);
  island.max_img_id = static_cast<unsigned>(right_lim);
  island.img_id = static_cast<unsigned>(img_index);
  island.score = 0.0;

  // Resetting weights
  clearWeights();
}

void Particle::clearWeights() {
  weight = 0.0f;
  weight_norm = 0.0f;
}

unsigned Particle::generateRandomMovement() {
  float rand_val = Random::get(0.0f, 1.0f);
  unsigned disp;
  if (rand_val < 0.5) {
    disp = 0;
  } else if (rand_val < 0.8) {
    disp = 1;
  } else if (rand_val < 0.95) {
    disp = 2;
  } else {
    disp = 3;
  }

  return disp;
}

}  // namespace ibow_lcd
