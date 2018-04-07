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

#include <exception>
#include <stdexcept>
#include <string>

namespace roa {
    class configuration_exception : public std::runtime_error {
    public:
        explicit configuration_exception(std::string error) : runtime_error(error) {}

        configuration_exception() : runtime_error("") {}
    };

    class kafka_exception : public std::runtime_error {
    public:
        explicit kafka_exception(std::string error) : runtime_error(error) {}

        kafka_exception() : runtime_error("") {}
    };

    class serialization_exception : public std::runtime_error {
    public:
        explicit serialization_exception(std::string error) : runtime_error(error) {}

        serialization_exception() : runtime_error("") {}
    };
}