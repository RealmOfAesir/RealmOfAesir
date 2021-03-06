/*
    Realm of Aesir
    Copyright (C) 2016  Michael de Lang

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

#include <initializer_list>
#include <chrono>
#include <string>
namespace roa {
    time_t get_current_time() noexcept;

    /**
     * Convert a string to unsigned
     * @param str
     * @param idx
     * @param base
     * @return unsigned
     * @throws out_of_range if value in str is more than unsigned.max()
     */
    unsigned stou(std::string const &str, size_t *idx = 0, int base = 10);
}