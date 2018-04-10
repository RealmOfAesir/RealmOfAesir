/*
    RealmOfAesir
    Copyright (C) 2018  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "random_helper.h"

using namespace roa;

random_helper::random_helper() : _rng32(pcg_extras::seed_seq_from<std::random_device>()),
    _rng64(pcg_extras::seed_seq_from<std::random_device>()) { }

uint32_t random_helper::generate_single(uint32_t from, uint32_t end) {
    std::uniform_int_distribution<decltype(from)> uniform_dist(from, end);
    return uniform_dist(_rng32);
}

int32_t random_helper::generate_single(int32_t from, int32_t end) {
    std::uniform_int_distribution<decltype(from)> uniform_dist(from, end);
    return uniform_dist(_rng32);
}

uint64_t random_helper::generate_single(uint64_t from, uint64_t end) {
    std::uniform_int_distribution<decltype(from)> uniform_dist(from, end);
    return uniform_dist(_rng64);
}

int64_t random_helper::generate_single(int64_t from, int64_t end) {
    std::uniform_int_distribution<decltype(from)> uniform_dist(from, end);
    return uniform_dist(_rng64);
}

float random_helper::generate_single(float from, float end) {
    std::uniform_real_distribution<decltype(from)> uniform_dist(from, end);
    return uniform_dist(_rng32);
}

double random_helper::generate_single(double from, double end) {
    std::uniform_real_distribution<decltype(from)> uniform_dist(from, end);
    return uniform_dist(_rng64);
}
