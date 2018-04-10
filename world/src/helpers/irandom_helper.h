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

#include <cstdint>

namespace roa {
    class irandom_helper {
    public:
        virtual ~irandom_helper() = default;


        virtual uint32_t generate_single(uint32_t from, uint32_t end) = 0;
        virtual int32_t generate_single(int32_t from, int32_t end) = 0;
        virtual uint64_t generate_single(uint64_t from, uint64_t end) = 0;
        virtual int64_t generate_single(int64_t from, int64_t end) = 0;
        virtual float generate_single(float from, float end) = 0;
        virtual double generate_single(double from, double end) = 0;
    };
}

