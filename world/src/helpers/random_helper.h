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


#pragma once

#include "irandom_helper.h"
#include <pcg_random.hpp>

namespace roa {

    class random_helper : public irandom_helper {
    public:
        random_helper();

        uint32_t generate_single(uint32_t from, uint32_t end) override;
        int32_t generate_single(int32_t from, int32_t end) override;
        uint64_t generate_single(uint64_t from, uint64_t end) override;
        int64_t generate_single(int64_t from, int64_t end) override;
        float generate_single(float from, float end) override;
        double generate_single(double from, double end) override;
    private:
        pcg32 _rng32;
        pcg64 _rng64;
    };
}
