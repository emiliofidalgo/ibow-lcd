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
  // Moving island's index
  unsigned disp_img = generateRandomMovementImage();
  island.img_id += disp_img;

  if (island.img_id > static_cast<unsigned>(nimages - 1)) {
    island.img_id = nimages - 1;
  }

  // Moving left limit
  // int disp_left = generateRandomMovementDisp();
  // int new_idx = static_cast<int>(island.min_img_id + disp_img) + disp_left;
  // FIXME static islands size!
  int new_idx = static_cast<int>(island.min_img_id + disp_img);

  if (new_idx < 0) {
    new_idx = 0;
  } else if (new_idx > static_cast<int>(island.img_id)) {
    new_idx = island.img_id;
  }

  island.min_img_id = static_cast<unsigned>(new_idx);

  // Moving right limit
  // int disp_right = generateRandomMovementDisp();
  // new_idx = static_cast<int>(island.max_img_id + disp_img) + disp_right;
  new_idx = static_cast<int>(island.max_img_id + disp_img);

  if (new_idx > nimages - 1) {
    new_idx = nimages - 1;
  } else if (new_idx < static_cast<int>(island.img_id)) {
    new_idx = island.img_id;
  }

  island.max_img_id = static_cast<unsigned>(new_idx);
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

unsigned Particle::generateRandomMovementImage() {
  float rand_val = Random::get(0.0f, 1.0f);
  unsigned disp;
  if (rand_val < 0.7f) {
    disp = 0;
  } else if (rand_val < 0.85f) {
    disp = 1;
  } else if (rand_val < 0.95f) {
    disp = 2;
  } else {
    disp = 3;
  }
  return disp;
}

int Particle::generateRandomMovementDisp() {
  float rand_val = Random::get(0.0f, 1.0f);
  int disp;
  if (rand_val < 0.05f) {
    disp = -2;
  } else if (rand_val < 0.15f) {
    disp = -1;
  } else if (rand_val < 0.85f) {
    disp = 0;
  } else if (rand_val < 0.95f) {
    disp = 1;
  } else {
    disp = 2;
  }
  return disp;
}

}  // namespace ibow_lcd
